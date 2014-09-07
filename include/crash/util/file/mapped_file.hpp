#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <boost/optional/optional.hpp>
#include <crash/util/file/file.hpp>

namespace crash {
namespace util {

class MappedFile : public File {
public:
   MappedFile(const std::string& path);
   virtual ~MappedFile();

   std::size_t size() const;
   int adviseMethod() const;
   void* data() const;

   std::size_t resize(std::size_t size);
   bool advise(int method);
   bool flush();

   static boost::optional< std::shared_ptr< MappedFile > >
    New(const std::string& path);
   static bool Delete(std::shared_ptr< MappedFile > file);

protected:
   std::size_t _size;
   void* _data;
   int _adviseMethod;

   static std::size_t bufferCeil(std::size_t size);

   static std::unordered_map< std::string, std::shared_ptr< MappedFile > >
    _instances;
   static std::mutex _instanceMutex;

   static const int _protFlags;
   static const int _mapFlags;
};

} // namespace util
} // namespace crash
