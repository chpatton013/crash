#pragma once

#include <sys/types.h>
#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>

namespace crash {
namespace util {

class File {
public:
   File(const boost::filesystem::path& path);
   virtual ~File();

   bool valid() const;
   const boost::filesystem::path& path() const;
   int handle() const;
   virtual size_t size() const;

protected:
   template< typename T >
   static boost::optional< std::shared_ptr< T > > New(
    const std::string& fileName,
    std::unordered_map< std::string, std::shared_ptr< T > >& instances);

   template< typename T >
   static bool Delete(std::shared_ptr< T > file,
    std::unordered_map< std::string, std::shared_ptr< T > >& instances);

   bool _valid;
   boost::filesystem::path _path;
   int _handle;

   static const int _flags;
   static const mode_t _mode;
};

} // namespace util
} // namespace crash

#include <crash/util/file/file.inl>
