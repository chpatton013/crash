#include <GLFW/glfw3.h>
#include <crash/window/monitor.hpp>

using namespace crash::window;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Monitor::Monitor() :
   Monitor(Monitor::getPrimaryHandle())
{}

Monitor::Monitor(const Monitor& monitor) :
   Monitor(monitor._handle)
{}

Monitor::Monitor(GLFWmonitor* handle) :
   _handle(handle)
{}

////////////////////////////////////////////////////////////////////////////////
// GLFW callthroughs.
////////////////////////////////////////////////////////////////////////////////

GLFWmonitor* Monitor::getHandle() const {
   return this->_handle;
}

std::string Monitor::getName() const {
   const char* name = glfwGetMonitorName(this->_handle);
   if (name == nullptr) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   return std::string(name);
}

glm::ivec2 Monitor::getPosition() const {
   int x = -1, y = -1;
   glfwGetMonitorPos(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Monitor::getSize() const {
   int width = -1, height = -1;

   glfwGetMonitorPhysicalSize(this->_handle, &width, &height);
   if (width <= 0 && height <= 0) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   return glm::ivec2(width, height);
}

std::vector< const GLFWvidmode* > Monitor::getAvailableModes() const {
   int count = -1;
   const GLFWvidmode* modes = glfwGetVideoModes(this->_handle, &count);
   if (modes == nullptr || count <= 0) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   std::vector< const GLFWvidmode* > availableModes;
   for (int ndx = 0; ndx < count; ++ndx) {
      availableModes.push_back(modes + ndx);
   }
   return availableModes;
}

const GLFWvidmode* Monitor::getMode() const {
   const GLFWvidmode* mode = glfwGetVideoMode(this->_handle);
   if (mode == nullptr) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   return mode;
}

const GLFWgammaramp* Monitor::getGamma() const {
   const GLFWgammaramp* gammaRamp = glfwGetGammaRamp(this->_handle);
   if (gammaRamp == nullptr) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   return gammaRamp;
}

void Monitor::setGammaExponent(float gammaExponent) const {
   glfwSetGamma(this->_handle, gammaExponent);
}

void Monitor::setGammaRamp(const GLFWgammaramp* gammaRamp) const {
   glfwSetGammaRamp(this->_handle, gammaRamp);
}

////////////////////////////////////////////////////////////////////////////////
// Static accessors.
////////////////////////////////////////////////////////////////////////////////

/* static */ GLFWmonitor* Monitor::getPrimaryHandle() {
   return glfwGetPrimaryMonitor();
}

/* static */ std::vector< GLFWmonitor* > Monitor::getAvailableHandles() {
   int count = -1;
   GLFWmonitor** handles = glfwGetMonitors(&count);
   if (handles == nullptr || count <= 0) {
      return std::vector< GLFWmonitor* >();
   }

   std::vector< GLFWmonitor* > availableHandles;
   for (int ndx = 0; ndx < count; ++ndx) {
      availableHandles.push_back(handles[ndx]);
   }

   return availableHandles;
}

/* static */ boost::optional< Monitor > Monitor::getPrimaryMonitor() {
   GLFWmonitor* primaryHandle = Monitor::getPrimaryHandle();
   if (primaryHandle == nullptr) {
      return boost::none;
   }

   return Monitor(primaryHandle);
}

/* static */ boost::optional< std::vector< Monitor > > Monitor::getAvailableMonitors() {
   auto handles = Monitor::getAvailableHandles();
   if (handles.empty()) {
      return boost::none;
   }

   std::vector< Monitor > availableMonitors;
   for (auto handle : handles) {
      availableMonitors.push_back(Monitor(handle));
   }

   return availableMonitors;
}

////////////////////////////////////////////////////////////////////////////////
// Static initializers.
////////////////////////////////////////////////////////////////////////////////

/* static */ void Monitor::initialize() {
   if (Monitor::_initialized) {
      return;
   }

   glfwSetMonitorCallback(Monitor::monitorCallbackAdapter);
   Monitor::_monitorCb = nullptr;

   Monitor::_initialized = true;
}

/* static */ void Monitor::teardown() {
   if (!Monitor::_initialized) {
      return;
   }

   glfwSetMonitorCallback(nullptr);
   Monitor::_monitorCb = nullptr;

   Monitor::_initialized = false;
}

////////////////////////////////////////////////////////////////////////////////
// Callback management.
////////////////////////////////////////////////////////////////////////////////

/* static */ boost::optional< Monitor::monitorCallback >
 Monitor::getMonitorCallback() {
   if (Monitor::_monitorCb == nullptr) {
      return boost::none;
   }

   return Monitor::_monitorCb;
}

/* static */ boost::optional< Monitor::monitorCallback >
 Monitor::removeMonitorCallback() {
   return Monitor::setMonitorCallback(nullptr);
}

/* static */ boost::optional< Monitor::monitorCallback >
 Monitor::setMonitorCallback(monitorCallback callback) {
   Monitor::monitorCallback oldCallback = Monitor::_monitorCb;
   Monitor::_monitorCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

/* static */ void Monitor::monitorCallbackAdapter(GLFWmonitor* handle, int state) {
   if (Monitor::_monitorCb != nullptr) {
      Monitor::_monitorCb(Monitor(handle), state);
   }
}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ Monitor::monitorCallback Monitor::_monitorCb;

/* static */ bool Monitor::_initialized = false;

Monitor::initializer::initializer() {
   Monitor::initialize();
}

/* static */ Monitor::initializer Monitor::_initializer;
