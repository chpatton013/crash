#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <algorithm>
#include <memory>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>
#include <crash/util/file/sync_file.hpp>
#include <crash/util/file/mapped_file.hpp>
#include <catch.hpp>

#ifndef TMPDIR
#define TMPDIR "/tmp/crash"
#endif

namespace fs = boost::filesystem;
namespace sys = boost::system;

using namespace crash::util;

std::string uniquePath(const std::string& prefix) {
   sys::error_code errorCode;

   std::string pathModel = prefix + "/%%%%-%%%%-%%%%-%%%%";
   fs::path dirPath = fs::unique_path(pathModel, errorCode);
   REQUIRE(errorCode.value() == 0);

   return dirPath.string();
}

std::string mkDir() {
   fs::path dirPath = fs::path(uniquePath(TMPDIR));
   sys::error_code errorCode;

   fs::create_directory(TMPDIR, errorCode);

   REQUIRE(fs::create_directory(dirPath, errorCode));
   REQUIRE(errorCode.value() == 0);

   return dirPath.string();
}

std::string mkFile(const std::string& dir) {
   std::string fileName = uniquePath(dir);

   int fd = ::open(fileName.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   REQUIRE(fd != -1);
   REQUIRE(::close(fd) != -1);

   return fileName;
}

std::string mkSymlink(const std::string& source, const std::string& dir) {
   std::string dest = uniquePath(dir);
   sys::error_code errorCode;

   fs::create_symlink(fs::path(source), fs::path(dest), errorCode);
   REQUIRE(errorCode.value() == 0);

   return dest;
}

template < typename T >
bool bufferContains(T* buffer, T value, std::size_t length) {
   for (int ndx = 0; ndx < length; ++ndx) {
      if (buffer[ndx] != value) {
         return false;
      }
   }

   return true;
}

TEST_CASE("util/file/open/base") {
   std::string badPath = "/dev/null/bad";

   REQUIRE(!SyncFile::New(badPath));
   REQUIRE(!MappedFile::New(badPath));

   std::string directory = mkDir();
   std::string syncNonExistentPath = uniquePath(directory);
   std::string mappedNonExistentPath = uniquePath(directory);

   REQUIRE(SyncFile::New(syncNonExistentPath));
   REQUIRE(MappedFile::New(mappedNonExistentPath));

   std::string syncExistentPath = mkFile(directory);
   std::string mappedExistentPath = mkFile(directory);

   REQUIRE(SyncFile::New(syncExistentPath));
   REQUIRE(MappedFile::New(mappedExistentPath));
}

TEST_CASE("util/file/open/singleton") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);

   CAUTO syncFileOpt1 = SyncFile::New(fileName);
   CAUTO syncFileOpt2 = SyncFile::New(fileName);

   REQUIRE(syncFileOpt1);
   REQUIRE(syncFileOpt2);
   REQUIRE(syncFileOpt1.get() == syncFileOpt2.get());

   CAUTO mappedFileOpt1 = MappedFile::New(fileName);
   CAUTO mappedFileOpt2 = MappedFile::New(fileName);

   REQUIRE(mappedFileOpt1);
   REQUIRE(mappedFileOpt2);
   REQUIRE(mappedFileOpt1.get() == mappedFileOpt2.get());
}

TEST_CASE("util/file/open/canonical") {
   std::string directory = mkDir();
   std::string realFileName = mkFile(directory);
   std::string linkFileName = mkSymlink(realFileName, directory);

   CAUTO syncRealFileOpt = SyncFile::New(realFileName);
   CAUTO syncLinkFileOpt = SyncFile::New(linkFileName);

   REQUIRE(syncRealFileOpt);
   REQUIRE(syncLinkFileOpt);
   REQUIRE(syncRealFileOpt.get() == syncLinkFileOpt.get());

   CAUTO mappedRealFileOpt = MappedFile::New(realFileName);
   CAUTO mappedLinkFileOpt = MappedFile::New(linkFileName);

   REQUIRE(mappedRealFileOpt);
   REQUIRE(mappedLinkFileOpt);
   REQUIRE(mappedRealFileOpt.get() == mappedLinkFileOpt.get());
}

TEST_CASE("util/file/empty/read") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const std::size_t N = 100;
   int buffer[N];
   ::memset(buffer, -1, sizeof(int) * N);

   CAUTO syncFileOpt = SyncFile::New(fileName);
   REQUIRE(syncFileOpt);
   AUTO syncFile = syncFileOpt.get();

   REQUIRE(bufferContains(buffer, -1, N));
   REQUIRE(syncFile->read(buffer, N) == 0);
   REQUIRE(bufferContains(buffer, -1, N));

   CAUTO mappedFileOpt = MappedFile::New(fileName);
   REQUIRE(mappedFileOpt);
   AUTO mappedFile = mappedFileOpt.get();

   mappedFile->resize(N);
   REQUIRE(bufferContains((char*)mappedFile->data(), '\0', N));
}

TEST_CASE("util/file/empty/write-read") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const std::size_t N = 100;
   char writeBuffer[N];
   char readBuffer[N];
   ::memset(writeBuffer, 1, sizeof(char) * N);

   CAUTO fileOpt = SyncFile::New(fileName);
   REQUIRE(fileOpt);
   AUTO file = fileOpt.get();

   REQUIRE(file->write(writeBuffer, N) == N);
   file->seek(0, SEEK_SET);
   REQUIRE(file->read(readBuffer, N) == N);

   REQUIRE(bufferContains(readBuffer, (char)1, N));
}

TEST_CASE("util/file/size/new") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);

   CAUTO syncFileOpt = SyncFile::New(fileName);
   REQUIRE(syncFileOpt);

   AUTO syncFile = syncFileOpt.get();
   REQUIRE(syncFile->size() == 0);

   CAUTO mappedFileOpt = MappedFile::New(fileName);
   REQUIRE(mappedFileOpt);

   AUTO mappedFile = mappedFileOpt.get();
   REQUIRE(mappedFile->size() == ::getpagesize());
}

TEST_CASE("util/file/sync/size/existent") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const std::size_t N = 100;
   char buffer[N];
   ::memset(buffer, -1, sizeof(char) * N);

   boost::optional< std::shared_ptr< SyncFile > > fileOpt =
    SyncFile::New(fileName);
   REQUIRE(fileOpt);
   std::shared_ptr< SyncFile > file = fileOpt.get();

   REQUIRE(file->write(buffer, N) == N);
   REQUIRE(file->sync());

   // Close and reopen file.
   SyncFile::Delete(file);
   fileOpt = SyncFile::New(fileName);
   REQUIRE(fileOpt);
   file = fileOpt.get();

   REQUIRE(file->size() == N);
}

TEST_CASE("util/file/sync/seek-tell") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const off_t N = 100;

   CAUTO fileOpt = SyncFile::New(fileName);
   REQUIRE(fileOpt);
   AUTO file = fileOpt.get();

   file->seek(N, SEEK_CUR);
   REQUIRE(file->tell() == N);
}

TEST_CASE("util/file/sync/seek-read-write-tell") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const off_t N = 100;
   char buffer[N];

   CAUTO fileOpt = SyncFile::New(fileName);
   REQUIRE(fileOpt);
   AUTO file = fileOpt.get();

   file->write(buffer, N);
   REQUIRE(file->tell() == N);

   file->seek(N, SEEK_CUR);
   file->write(buffer, 1);
   REQUIRE(file->size() == 2 * N + 1);

   file->seek(-N, SEEK_CUR);
   REQUIRE(file->tell() == N + 1);

   file->read(buffer, N);
   REQUIRE(file->tell() == 2 * N + 1);
}

TEST_CASE("util/file/mapped/size/existent") {
   std::string directory = mkDir();
   std::string fileName = mkFile(directory);
   const std::size_t N = 100;

   boost::optional< std::shared_ptr< MappedFile > > fileOpt =
    MappedFile::New(fileName);
   REQUIRE(fileOpt);
   std::shared_ptr< MappedFile > file = fileOpt.get();

   std::size_t numBytes = file->resize(N);
   REQUIRE(file->flush());

   // Close and reopen file.
   MappedFile::Delete(file);
   fileOpt = MappedFile::New(fileName);
   REQUIRE(fileOpt);
   file = fileOpt.get();

   REQUIRE(file->size() == numBytes);
}
