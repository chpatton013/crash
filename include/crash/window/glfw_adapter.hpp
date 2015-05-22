#pragma once

#include <exception>
#include <memory>
#include <string>
#include <boost/optional/optional.hpp>

namespace crash {
namespace window {

struct GlfwInitializer {
   GlfwInitializer();
   virtual ~GlfwInitializer();
};

class GlfwAdapter {
public:
   typedef void (*errorCallback)(int, std::string);

   GlfwAdapter();
   virtual ~GlfwAdapter();

   static boost::optional< errorCallback > getErrorCallback();
   static boost::optional< errorCallback > removeErrorCallback();
   static boost::optional< errorCallback > setErrorCallback(errorCallback callback);
   static void errorCallbackAdapter(int error, const char* description);

   struct InvalidGlfwState : public std::exception {
      InvalidGlfwState();
      const char* what() const noexcept;
   };

private:
   static errorCallback _errorCb;
   static std::shared_ptr< GlfwInitializer > _glfwInitializer;

   static struct initializer {
      initializer();
   } _initializer;
};

} // namespace window
} // namespace crash
