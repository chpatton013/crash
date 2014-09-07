#include <crash/util/file/sync_file.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <functional>
#include <boost/functional/hash.hpp>

using namespace crash::util;

SyncFile::SyncFile(const std::string& path) :
   File(path)
{}

SyncFile::~SyncFile() {}

off_t SyncFile::seek(off_t offset, int whence) {
   return ::lseek(this->_handle, offset, whence);
}

off_t SyncFile::tell() {
   return ::lseek(this->_handle, 0, SEEK_CUR);
}

size_t SyncFile::read(void* buffer, size_t bytes) {
   return ::read(this->_handle, buffer, bytes);
}

size_t SyncFile::write(const void* buffer, size_t bytes) {
   return ::write(this->_handle, buffer, bytes);
}

bool SyncFile::sync() {
   return ::fcntl(this->_handle, F_FULLFSYNC) == 0;
}

// static
boost::optional< std::shared_ptr< SyncFile > >
 SyncFile::New(const std::string& fileName) {
   std::lock_guard<std::mutex> lock(SyncFile::_instanceMutex);

   return File::New< SyncFile >(fileName, SyncFile::_instances);
}

// static
bool SyncFile::Delete(std::shared_ptr< SyncFile > file) {
   std::lock_guard<std::mutex> lock(SyncFile::_instanceMutex);

   return File::Delete< SyncFile >(file, SyncFile::_instances);
}

// static
std::unordered_map< std::string, std::shared_ptr< SyncFile > > SyncFile::_instances;
// static
std::mutex SyncFile::_instanceMutex;
