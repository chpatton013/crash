#pragma once

#include <sys/types.h>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <crash/util/file/file.hpp>
#include <boost/optional/optional.hpp>

namespace crash {
namespace util {

class SyncFile : public File {
public:
   SyncFile(const std::string& path);
   virtual ~SyncFile();

   off_t seek(off_t offset, int whence);
   off_t tell();

   size_t read(void* buffer, size_t bytes);
   size_t write(const void* buffer, size_t bytes);
   bool sync();

   static boost::optional< std::shared_ptr< SyncFile > >
    New(const std::string& path);
   static bool Delete(std::shared_ptr< SyncFile > file);

protected:
   static std::unordered_map< std::string, std::shared_ptr< SyncFile > >
    _instances;
   static std::mutex _instanceMutex;
};

} // namespace util
} // namespace crash
