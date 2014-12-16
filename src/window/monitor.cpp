#include <cstdio>
#include <iostream>
#include <crash/window/monitor.hpp>

using namespace crash::window;

void crash::window::print(const Monitor& monitor) {
   print(monitor, true);
}

void crash::window::print(const Monitor& monitor, bool newline) {
   const char* formatString;
   if (newline) {
      formatString = "Monitor; name(%s); position(%d,%d); size(%d,%d)\n";
   } else {
      formatString = "Monitor; name(%s); position(%d,%d); size(%d,%d)";
   }
   printf(formatString, monitor.name().data(),
    monitor.position().x, monitor.position().y,
    monitor.size().x, monitor.size().y);
}

void crash::window::print(const GLFWvidmode* mode) {
   print(mode, true);
}

void crash::window::print(const GLFWvidmode* mode, bool newline) {
   const char* formatString;
   if (newline) {
      formatString = "GLFWvidmode; size(%d,%d); color(%d,%d,%d); refresh(%d)\n";
   } else {
      formatString = "GLFWvidmode; size(%d,%d); color(%d,%d,%d); refresh(%d)";
   }
   printf(formatString, mode->width, mode->height,
    mode->redBits, mode->greenBits, mode->blueBits,
    mode->refreshRate);
}

void crash::window::print(const GLFWgammaramp* gamma) {
   print(gamma, true);
}

void crash::window::print(const GLFWgammaramp* gamma, bool newline) {
   std::cout << "GLFWgammaramp; size(" << gamma->size << ");";

   if (gamma->size > 0) {
      std::cout << " red(" << gamma->red[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->red[ndx];
         }
      }
      std::cout << ");";

      std::cout << " green(" << gamma->green[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->green[ndx];
         }
      }
      std::cout << ");";

      std::cout << " blue(" << gamma->blue[0];
      if (gamma->size > 1) {
         for (unsigned int ndx = 1; ndx < gamma->size; ++ndx) {
            std::cout << "," << gamma->blue[ndx];
         }
      }
      std::cout << ");";
   }

   if (newline) {
      std::cout << std::endl;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////////////////////

GLFWmonitor* Monitor::handle() const { return this->_handle; }

std::string Monitor::name() const {
   const char* name = glfwGetMonitorName(this->_handle);
   if (name == nullptr) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   return std::string(name);
}

glm::ivec2 Monitor::position() const {
   int x = -1, y = -1;
   glfwGetMonitorPos(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Monitor::size() const {
   int width = -1, height = -1;

   glfwGetMonitorPhysicalSize(this->_handle, &width, &height);
   if (width == 0 && height == 0) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   return glm::ivec2(width, height);
}

std::vector< const GLFWvidmode* > Monitor::availableModes() const {
   int count = -1;
   const GLFWvidmode* modes = glfwGetVideoModes(this->_handle, &count);
   if (modes == nullptr || count <= 0) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   std::vector< const GLFWvidmode* > availableModes;
   for (int ndx = 0; ndx < count; ++ndx) {
      availableModes.push_back(modes + ndx);
   }
   return availableModes;
}

const GLFWvidmode* Monitor::mode() const {
   const GLFWvidmode* mode = glfwGetVideoMode(this->_handle);
   if (mode == nullptr) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   return mode;
}

const GLFWgammaramp* Monitor::gamma() const {
   const GLFWgammaramp* gammaRamp = glfwGetGammaRamp(this->_handle);
   if (gammaRamp == nullptr) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   return gammaRamp;
}

////////////////////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////////////////////

void Monitor::gamma(float gammaExponent) const {
   glfwSetGamma(this->_handle, gammaExponent);
}

void Monitor::gamma(const GLFWgammaramp* gammaRamp) const {
   glfwSetGammaRamp(this->_handle, gammaRamp);
}

////////////////////////////////////////////////////////////////////////////////
// Static constructors.
////////////////////////////////////////////////////////////////////////////////

//static
boost::optional< Monitor > Monitor::factory() {
   return Monitor::factory(nullptr);
}

/* static */ boost::optional< Monitor > Monitor::factory(GLFWmonitor* handle) {
   if (handle == nullptr) {
      auto handleOpt = Monitor::primaryHandle();
      if (!handleOpt) {
         return boost::none;
      }

      handle = handleOpt.get();
   }

   auto itr = Monitor::_instances.find(handle);
   if (itr != Monitor::_instances.end()) {
      return *itr->second;
   }

   Monitor* monitor = new Monitor(handle);
   Monitor::_instances[handle] = monitor;
   return *monitor;
}

/* static */ bool Monitor::release(Monitor monitor) {
   return Monitor::_instances.erase(monitor._handle);
}

////////////////////////////////////////////////////////////////////////////////
// Static getters.
////////////////////////////////////////////////////////////////////////////////

/* static */ boost::optional< GLFWmonitor* > Monitor::primaryHandle() {
   GLFWmonitor* handle = glfwGetPrimaryMonitor();
   if (handle == nullptr) {
      return boost::none;
   } else {
      return handle;
   }
}

/* static */ boost::optional< std::vector< GLFWmonitor* > > Monitor::availableHandles() {
   int count = -1;
   GLFWmonitor** handles = glfwGetMonitors(&count);

   if (handles == nullptr || count <= 0) {
      return boost::none;
   }

   std::vector< GLFWmonitor* > availableHandles;
   for (int ndx = 0; ndx < count; ++ndx) {
      availableHandles.push_back(handles[ndx]);
   }

   return availableHandles;
}

/* static */ boost::optional< Monitor > Monitor::primaryMonitor() {
   auto handle = Monitor::primaryHandle();
   if (!handle) {
      return boost::none;
   }

   auto itr = Monitor::_instances.find(handle.get());
   if (itr == Monitor::_instances.end()) {
      throw GlfwAdapter::_invalidInternalState;
   }

   return *itr->second;
}

/* static */ boost::optional< std::vector< Monitor > > Monitor::availableMonitors() {
   auto handles = Monitor::availableHandles();
   if (!handles) {
      return boost::none;
   }

   std::vector< Monitor > availableMonitors;
   for (auto handle : handles.get()) {
      auto itr = Monitor::_instances.find(handle);
      if (itr == Monitor::_instances.end()) {
         throw GlfwAdapter::_invalidInternalState;
      }

      availableMonitors.push_back(*itr->second);
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

   auto handles = Monitor::availableHandles();
   for (auto handle : handles.get()) {
      Monitor::factory(handle);
   }

   Monitor::_initialized = true;
}

/* static */ void Monitor::teardown() {
   if (!Monitor::_initialized) {
      return;
   }

   glfwSetMonitorCallback(nullptr);
   Monitor::_monitorCb = nullptr;

   for (auto itr : Monitor::_instances) {
      Monitor::release(*itr.second);
   }

   Monitor::_initialized = false;
}

////////////////////////////////////////////////////////////////////////////////
// Callback management.
////////////////////////////////////////////////////////////////////////////////

/* static */ boost::optional< Monitor::monitorCallback > Monitor::getMonitorCallback() {
   return Monitor::_monitorCb;
}

/* static */ boost::optional< Monitor::monitorCallback > Monitor::removeMonitorCallback() {
   return Monitor::setMonitorCallback(nullptr);
}

/* static */ boost::optional< Monitor::monitorCallback >
 Monitor::setMonitorCallback(monitorCallback callback) {
   Monitor::monitorCallback oldCallback = Monitor::_monitorCb;
   Monitor::_monitorCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

/* static */ void Monitor::monitorCallbackAdapter(GLFWmonitor* handle, int state) {
   auto itr = Monitor::_instances.find(handle);
   if (itr == Monitor::_instances.end()) {
      return;
   }

   auto monitor = itr->second;
   if (monitor->_monitorCb == nullptr) {
      return;
   }

   monitor->_monitorCb(*monitor, state);
}

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Monitor::Monitor() :
   Monitor(nullptr)
{}

Monitor::Monitor(GLFWmonitor* handle) :
   _handle(handle)
{}

/* virtual */ Monitor::~Monitor() {}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ Monitor::monitorCallback Monitor::_monitorCb;

/* static */ std::map< GLFWmonitor*, Monitor* > Monitor::_instances;

/* static */ bool Monitor::_initialized = false;

Monitor::initializer::initializer() {
   Monitor::initialize();
}

/* static */ Monitor::initializer Monitor::_initializer;
