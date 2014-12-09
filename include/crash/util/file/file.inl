#pragma once

#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>

namespace crash {
namespace util {

// static
template< typename T >
boost::optional< std::shared_ptr< T > > File::New(const std::string& fileName,
 std::unordered_map< std::string, std::shared_ptr< T > >& instances) {
   static boost::optional< std::shared_ptr< T > > none;

   namespace fs = boost::filesystem;
   namespace sys = boost::system;

   sys::error_code errorCode;
   fs::path filePath = fs::path(fileName);

   if (!fs::exists(filePath, errorCode)) {
      std::shared_ptr< T > file = std::make_shared< T >(fileName);
      if (!file->valid()) {
         return none;
      }

      return instances[fileName] = file;
   }

   bool isSymLink = fs::is_symlink(filePath, errorCode);
   if (errorCode.value() != 0) {
      return none;
   }

   if (isSymLink) {
      filePath = fs::read_symlink(filePath, errorCode);
      if (errorCode.value() != 0) {
         return none;
      }

      return File::New(filePath.string(), instances);
   }

   fs::path absoluteFilePath = fs::absolute(filePath);
   std::string absoluteFileName = absoluteFilePath.string();

   auto itr = instances.find(absoluteFileName);
   if (itr != instances.end()) {
      return itr->second;
   }

   bool isRegularFile = fs::is_regular_file(absoluteFilePath, errorCode);
   if (errorCode.value() != 0 || !isRegularFile) {
      return none;
   }

   std::shared_ptr< T > file = std::make_shared< T >(absoluteFileName);
   if (!file->valid()) {
      return none;
   }

   return instances[absoluteFileName] = file;
}

// static
template< typename T >
bool File::Delete(std::shared_ptr< T > file,
 std::unordered_map< std::string, std::shared_ptr< T > >& instances) {
   auto itr = instances.find(file->path().string());
   if (itr == instances.end()) {
      return false;
   }

   // Release resources.
   itr->second.reset();
   instances.erase(itr);

   return true;
}

} // namespace util
} // namespace crash
