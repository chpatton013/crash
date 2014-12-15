#include <GLFW/glfw3.h>
#include <crash/window/glfw_adapter.hpp>

using namespace crash::window;

GlfwInitializer::GlfwInitializer() {
   glfwInit();
}

/* virtual */ GlfwInitializer::~GlfwInitializer() {
   glfwTerminate();
}

GlfwAdapter::GlfwAdapter() {}

/* virtual */ GlfwAdapter::~GlfwAdapter() {}

/* static */ boost::optional< GlfwAdapter::errorCallback > GlfwAdapter::getErrorCallback() {
   if (GlfwAdapter::_errorCb == nullptr) {
      return boost::none;
   } else {
      return GlfwAdapter::_errorCb;
   }
}

/* static */ boost::optional< GlfwAdapter::errorCallback >
 GlfwAdapter::removeErrorCallback() {
   return GlfwAdapter::setErrorCallback(nullptr);
}

/* static */ boost::optional< GlfwAdapter::errorCallback >
 GlfwAdapter::setErrorCallback(GlfwAdapter::errorCallback callback) {
   GlfwAdapter::errorCallback oldCallback = GlfwAdapter::_errorCb;
   GlfwAdapter::_errorCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

/* static */ void GlfwAdapter::errorCallbackAdapter(int error, const char* description) {
   if (GlfwAdapter::_errorCb == nullptr) {
      return;
   }

   GlfwAdapter::_errorCb(error, std::string(description));
}

/* static */ GlfwAdapter::InvalidGlfwState GlfwAdapter::_invalidGlfwState;

/* static */ GlfwAdapter::InvalidInternalState GlfwAdapter::_invalidInternalState;

/* static */ GlfwAdapter::errorCallback GlfwAdapter::_errorCb = nullptr;

/* static */ std::shared_ptr< GlfwInitializer > GlfwAdapter::_glfwInitializer =
 std::make_shared< GlfwInitializer >();

GlfwAdapter::initializer::initializer() {
   glfwSetErrorCallback(GlfwAdapter::errorCallbackAdapter);
}

/* static */ GlfwAdapter::initializer GlfwAdapter::_initializer;
