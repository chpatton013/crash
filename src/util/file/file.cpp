#include <crash/util/file/file.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <crash/util/type.hpp>

using namespace crash::util;

File::File(const boost::filesystem::path& path) :
   _path(path)
{
   this->_handle = ::open(path.string().data(), File::_flags, File::_mode);
   this->_valid = this->_handle != -1;
}

File::~File() {
   ::close(this->_handle);
}

bool File::valid() const { return this->_valid; }
const boost::filesystem::path& File::path() const { return this->_path; }
int File::handle() const { return this->_handle; }

size_t File::size() const {
   struct stat fileStat;
   if (::fstat(this->_handle, &fileStat) == -1) {
      return 0;
   }
   return fileStat.st_size;
}

// static
const int File::_flags = O_RDWR | O_CREAT;
// static
const mode_t File::_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
