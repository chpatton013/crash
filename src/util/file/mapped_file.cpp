#include <crash/util/file/mapped_file.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <algorithm>
#include <cmath>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

using namespace crash::util;

MappedFile::MappedFile(const std::string& path) :
   File(path)
{
   struct stat fileStat;
   if (::fstat(this->_handle, &fileStat) == -1) {
      this->_valid = false;
      return;
   }
   std::size_t size = MappedFile::bufferCeil(fileStat.st_size);
   ::ftruncate(this->_handle, size);

   void* data = ::mmap(nullptr, size, MappedFile::_protFlags,
    MappedFile::_mapFlags, this->_handle, 0);
   if (data == MAP_FAILED) {
      this->_valid = false;
      return;
   }

   this->_size = size;
   this->_data = data;
   this->_adviseMethod = MADV_NORMAL;
   this->_valid = true;
}

/* virtual */ MappedFile::~MappedFile() {
   this->flush();
   ::munmap(this->_data, this->_size);
}

std::size_t MappedFile::size() const { return this->_size; }
int MappedFile::adviseMethod() const { return this->_adviseMethod; }
void* MappedFile::data() const { return this->_data; }

std::size_t MappedFile::resize(std::size_t size) {
   if (size <= this->_size) {
      return this->_size;
   }

   size = MappedFile::bufferCeil(size);

   ::munmap(this->_data, this->_size);
   ::ftruncate(this->_handle, size);

   this->_data = ::mmap(nullptr, size, MappedFile::_protFlags,
    MappedFile::_mapFlags, this->_handle, 0);
   if (this->_data == MAP_FAILED) {
      this->_valid = false;
      return 0;
   }

   return this->_size = size;
}

bool MappedFile::advise(int method) {
   bool successful = ::madvise(this->_data, this->_size, method) == 0;
   if (successful) {
      this->_adviseMethod = method;
   }

   return successful;
}

bool MappedFile::flush() {
   return ::msync(this->_data, this->_size, MS_SYNC) == 0;
}

/* static */ boost::optional< std::shared_ptr< MappedFile > >
 MappedFile::New(const std::string& fileName) {
   std::lock_guard<std::mutex> lock(MappedFile::_instanceMutex);

   return File::New< MappedFile >(fileName, MappedFile::_instances);
}

/* static */ bool MappedFile::Delete(std::shared_ptr< MappedFile > file) {
   std::lock_guard<std::mutex> lock(MappedFile::_instanceMutex);

   return File::Delete< MappedFile >(file, MappedFile::_instances);
}

/* static */ std::size_t MappedFile::bufferCeil(std::size_t size) {
   static const std::size_t bufferSize = ::getpagesize();

   int buffers = std::max((int)std::ceil(size / (float)bufferSize), 1);
   return buffers * bufferSize;
}

/* static */ std::unordered_map< std::string, std::shared_ptr< MappedFile > > MappedFile::_instances;
/* static */ std::mutex MappedFile::_instanceMutex;

/* static */ const int MappedFile::_protFlags = PROT_READ | PROT_WRITE;
/* static */ const int MappedFile::_mapFlags = MAP_SHARED;
