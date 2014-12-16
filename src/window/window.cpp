#include <glm/glm.hpp>
#include <crash/window/keyboard.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/mouse.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Window::Window(const glm::ivec2& size) :
   Window(size, "")
{}

Window::Window(const glm::ivec2& size, const std::string& title) :
   Window(size, title, boost::none)
{}

Window::Window(const glm::ivec2& size, const std::string& title,
 boost::optional< Monitor > monitor) :
   Window(size, title, monitor, boost::none)
{}

Window::Window(const glm::ivec2& size, const std::string& title,
 boost::optional< Monitor > monitor, boost::optional< Window > share) :
   _destroyed(false), _title(title), _positionCb(nullptr), _windowSizeCb(nullptr),
   _closeCb(nullptr), _refreshCb(nullptr), _focusCb(nullptr), _minimizeCb(nullptr),
   _frameBufferSizeCb(nullptr)
{
   GLFWmonitor* monitorHandle = !monitor ? nullptr : monitor.get().handle();
   GLFWwindow* shareHandle = !share ? nullptr : share.get().handle();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   GLFWwindow* handle = glfwCreateWindow(size.x, size.y, title.data(),
    monitorHandle, shareHandle);
   if (handle == nullptr) {
      throw GlfwAdapter::_invalidGlfwState;
   }

   this->_handle = handle;

   glfwSetWindowPosCallback(handle, Window::positionCallbackAdapter);
   glfwSetWindowSizeCallback(handle, Window::windowSizeCallbackAdapter);
   glfwSetWindowCloseCallback(handle, Window::closeCallbackAdapter);
   glfwSetWindowRefreshCallback(handle, Window::refreshCallbackAdapter);
   glfwSetWindowFocusCallback(handle, Window::focusCallbackAdapter);
   glfwSetWindowIconifyCallback(handle, Window::minimizeCallbackAdapter);
   glfwSetFramebufferSizeCallback(handle, Window::frameBufferSizeCallbackAdapter);

   Window::_instances[handle] = this;
}

/* virtual */ Window::~Window() {
   this->destroy();
}

////////////////////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////////////////////

GLFWwindow* Window::handle() const {
   return this->_handle;
}

boost::optional< Monitor > Window::monitor() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   auto monitorHandle = glfwGetWindowMonitor(this->_handle);
   if (monitorHandle == nullptr) {
      return boost::none;
   }

   return Monitor::factory(monitorHandle);
}

std::string Window::title() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   return this->_title;
}

glm::ivec2 Window::position() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   int x, y;
   glfwGetWindowPos(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Window::windowSize() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   int x, y;
   glfwGetWindowSize(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Window::frameBufferSize() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   int x, y;
   glfwGetFramebufferSize(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

bool Window::visible() const {
   return this->attribute(GLFW_VISIBLE);
}

bool Window::focused() const {
   return this->attribute(GLFW_FOCUSED);
}

bool Window::minimized() const {
   return this->attribute(GLFW_ICONIFIED);
}

bool Window::resizable() const {
   return this->attribute(GLFW_RESIZABLE);
}

bool Window::decorated() const {
   return this->attribute(GLFW_DECORATED);
}

bool Window::shouldClose() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   return glfwWindowShouldClose(this->_handle);
}

int Window::attribute(int a) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   return glfwGetWindowAttrib(this->_handle, a);
}

int Window::inputMode(int m) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   return glfwGetInputMode(this->_handle, m);
}

void* Window::userPointer() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   return glfwGetWindowUserPointer(this->_handle);
}

bool Window::destroyed() const { return this->_destroyed; }

boost::optional< Window::positionCallback > Window::getPositionCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_positionCb == nullptr) {
      return boost::none;
   } else {
      return this->_positionCb;
   }
}

boost::optional< Window::windowSizeCallback > Window::getWindowSizeCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_windowSizeCb == nullptr) {
      return boost::none;
   } else {
      return this->_windowSizeCb;
   }
}

boost::optional< Window::closeCallback > Window::getCloseCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_closeCb == nullptr) {
      return boost::none;
   } else {
      return this->_closeCb;
   }
}

boost::optional< Window::refreshCallback > Window::getRefreshCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_refreshCb == nullptr) {
      return boost::none;
   } else {
      return this->_refreshCb;
   }
}

boost::optional< Window::focusCallback > Window::getFocusCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_focusCb == nullptr) {
      return boost::none;
   } else {
      return this->_focusCb;
   }
}

boost::optional< Window::minimizeCallback > Window::getMinimizeCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_minimizeCb == nullptr) {
      return boost::none;
   } else {
      return this->_minimizeCb;
   }
}

boost::optional< Window::frameBufferSizeCallback > Window::getFrameBufferCallback() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (this->_frameBufferSizeCb == nullptr) {
      return boost::none;
   } else {
      return this->_frameBufferSizeCb;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////////////////////

void Window::title(const std::string& t) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetWindowTitle(this->_handle, t.data());
   // There is no glfwGetWindowTitle so we have to hold on to the new value.
   this->_title = t;
}

void Window::position(const glm::ivec2& p) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetWindowPos(this->_handle, p.x, p.y);
}

void Window::windowSize(const glm::ivec2& s) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetWindowSize(this->_handle, s.x, s.y);
}

void Window::visible(bool v) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (v) {
      glfwShowWindow(this->_handle);
   } else {
      glfwHideWindow(this->_handle);
   }
}

void Window::minimized(bool m) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (m) {
      glfwIconifyWindow(this->_handle);
   } else {
      glfwRestoreWindow(this->_handle);
   }
}

void Window::shouldClose(bool c) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetWindowShouldClose(this->_handle, c ? GL_TRUE : GL_FALSE);
}

void Window::inputMode(int m, int v) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetInputMode(this->_handle, m, v);
}

void Window::userPointer(void* p) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSetWindowUserPointer(this->_handle, p);
}

boost::optional< Window::positionCallback > Window::removePositionCallback() {
   return this->setPositionCallback(nullptr);
}

boost::optional< Window::windowSizeCallback > Window::removeWindowSizeCallback() {
   return this->setWindowSizeCallback(nullptr);
}

boost::optional< Window::closeCallback > Window::removeCloseCallback() {
   return this->setCloseCallback(nullptr);
}

boost::optional< Window::refreshCallback > Window::removeRefreshCallback() {
   return this->setRefreshCallback(nullptr);
}

boost::optional< Window::focusCallback > Window::removeFocusCallback() {
   return this->setFocusCallback(nullptr);
}

boost::optional< Window::minimizeCallback > Window::removeMinimizeCallback() {
   return this->setMinimizeCallback(nullptr);
}

boost::optional< Window::frameBufferSizeCallback > Window::removeFrameBufferCallback() {
   return this->setFrameBufferSizeCallback(nullptr);
}

boost::optional< Window::positionCallback >
 Window::setPositionCallback(Window::positionCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::positionCallback oldCallback = this->_positionCb;
   this->_positionCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::windowSizeCallback >
 Window::setWindowSizeCallback(Window::windowSizeCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::windowSizeCallback oldCallback = this->_windowSizeCb;
   this->_windowSizeCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::closeCallback >
 Window::setCloseCallback(Window::closeCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::closeCallback oldCallback = this->_closeCb;
   this->_closeCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::refreshCallback >
 Window::setRefreshCallback(Window::refreshCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::refreshCallback oldCallback = this->_refreshCb;
   this->_refreshCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::focusCallback >
 Window::setFocusCallback(Window::focusCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::focusCallback oldCallback = this->_focusCb;
   this->_focusCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::minimizeCallback >
 Window::setMinimizeCallback(Window::minimizeCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::minimizeCallback oldCallback = this->_minimizeCb;
   this->_minimizeCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::frameBufferSizeCallback >
 Window::setFrameBufferSizeCallback(Window::frameBufferSizeCallback callback) {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   Window::frameBufferSizeCallback oldCallback = this->_frameBufferSizeCb;
   this->_frameBufferSizeCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

////////////////////////////////////////////////////////////////////////////////
// GLFW callthroughs.
////////////////////////////////////////////////////////////////////////////////

void Window::swapBuffers() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSwapBuffers(this->_handle);
}

void Window::swapInterval(unsigned int interval) const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwSwapInterval(interval);
}

void Window::makeContextCurrent() const {
   if (this->_destroyed) {
      throw Window::_destroyedWindow;
   }

   glfwMakeContextCurrent(this->_handle);
}

void Window::destroy() {
   if (this->_destroyed) {
      return;
   }

   Keyboard::release(*this);
   Mouse::release(*this);
   Window::_instances.erase(this->_handle);

   glfwDestroyWindow(this->_handle);
   this->_destroyed = true;
}

////////////////////////////////////////////////////////////////////////////////
// Static setters.
////////////////////////////////////////////////////////////////////////////////

/* static */ void Window::setHint(int target, int hint) {
   glfwWindowHint(target, hint);
}

////////////////////////////////////////////////////////////////////////////////
// Static GLFW callthroughs.
////////////////////////////////////////////////////////////////////////////////

/* static */ boost::optional< Window* > Window::getCurrentContext() {
   auto handle = glfwGetCurrentContext();
   if (handle == nullptr) {
      return boost::none;
   }

   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return boost::none;
   }

   return itr->second;
}

////////////////////////////////////////////////////////////////////////////////
// Callback adapters.
////////////////////////////////////////////////////////////////////////////////

/* static */ void Window::positionCallbackAdapter(GLFWwindow* handle, int x, int y) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_positionCb == nullptr) {
      return;
   }

   window->_positionCb(*window, glm::ivec2(x, y));
}

/* static */ void Window::windowSizeCallbackAdapter(GLFWwindow* handle, int w, int h) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_windowSizeCb == nullptr) {
      return;
   }

   window->_windowSizeCb(*window, glm::ivec2(w, h));
}

/* static */ void Window::frameBufferSizeCallbackAdapter(GLFWwindow* handle, int w, int h) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_frameBufferSizeCb == nullptr) {
      return;
   }

   window->_frameBufferSizeCb(*window, glm::ivec2(w, h));
}

/* static */ void Window::focusCallbackAdapter(GLFWwindow* handle, int f) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_focusCb == nullptr) {
      return;
   }

   window->_focusCb(*window, f);
}

/* static */ void Window::minimizeCallbackAdapter(GLFWwindow* handle, int m) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_minimizeCb == nullptr) {
      return;
   }

   window->_minimizeCb(*window, m);
}

/* static */ void Window::refreshCallbackAdapter(GLFWwindow* handle) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_refreshCb == nullptr) {
      return;
   }

   window->_refreshCb(*window);
}

/* static */ void Window::closeCallbackAdapter(GLFWwindow* handle) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::_destroyedWindow;
   }

   if (window->_closeCb == nullptr) {
      return;
   }

   window->_closeCb(*window);
}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ Window::DestroyedWindow Window::_destroyedWindow;

/* static */ std::map< GLFWwindow*, Window* > Window::_instances;
