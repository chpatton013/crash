#include <GLFW/glfw3.h>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

Window::DestroyedWindow::DestroyedWindow() :
   std::exception()
{}

const char* Window::DestroyedWindow::what() const noexcept {
   return "Window has already been destroyed";
}

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Window::Window(const Window& window) :
   _destroyed(window._destroyed),
    _handle(window._handle),
    _share(window._share), _title(window._title),
    _windowPositionCb(window._windowPositionCb),
    _windowSizeCb(window._windowSizeCb),
    _closeCb(window._closeCb),
    _refreshCb(window._refreshCb),
    _focusCb(window._focusCb),
    _minimizeCb(window._minimizeCb),
    _frameBufferSizeCb(window._frameBufferSizeCb),
    _keyCb(window._keyCb),
    _charCb(window._charCb),
    _mouseButtonCb(window._mouseButtonCb),
    _mousePositionCb(window._mousePositionCb),
    _mouseEnterCb(window._mouseEnterCb),
    _mouseScrollCb(window._mouseScrollCb)
{}

Window::Window(const glm::ivec2& size, const std::string& title,
 boost::optional< Monitor > monitor, boost::optional< Window > share) :
   _destroyed(false), _title(title), _windowPositionCb(nullptr),
    _windowSizeCb(nullptr), _closeCb(nullptr), _refreshCb(nullptr),
    _focusCb(nullptr), _minimizeCb(nullptr), _frameBufferSizeCb(nullptr),
    _keyCb(nullptr), _charCb(nullptr), _mouseButtonCb(nullptr),
    _mousePositionCb(nullptr), _mouseEnterCb(nullptr), _mouseScrollCb(nullptr)
{
   GLFWmonitor* monitorHandle = !monitor ? nullptr : monitor.get().getHandle();
   GLFWwindow* shareHandle = !share ? nullptr : share.get().getHandle();

   glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
   GLFWwindow* handle = glfwCreateWindow(size.x, size.y, title.data(),
    monitorHandle, shareHandle);
   if (handle == nullptr) {
      throw GlfwAdapter::InvalidGlfwState();
   }

   this->_handle = handle;

   glfwSetWindowPosCallback(handle, Window::windowPositionCallbackAdapter);
   glfwSetWindowSizeCallback(handle, Window::windowSizeCallbackAdapter);
   glfwSetWindowCloseCallback(handle, Window::closeCallbackAdapter);
   glfwSetWindowRefreshCallback(handle, Window::refreshCallbackAdapter);
   glfwSetWindowFocusCallback(handle, Window::focusCallbackAdapter);
   glfwSetWindowIconifyCallback(handle, Window::minimizeCallbackAdapter);
   glfwSetFramebufferSizeCallback(handle,
    Window::frameBufferSizeCallbackAdapter);
   glfwSetKeyCallback(handle, Window::keyCallbackAdapter);
   glfwSetCharCallback(handle, Window::charCallbackAdapter);
   glfwSetMouseButtonCallback(handle, Window::mouseButtonCallbackAdapter);
   glfwSetCursorPosCallback(handle, Window::mousePositionCallbackAdapter);
   glfwSetCursorEnterCallback(handle, Window::mouseEnterCallbackAdapter);
   glfwSetScrollCallback(handle, Window::mouseScrollCallbackAdapter);

   Window::_instances[handle] = this;
}

/* virtual */ Window::~Window() {
   this->destroy();
}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

GLFWwindow* Window::getHandle() const {
   return this->_handle;
}

boost::optional< Monitor > Window::getMonitor() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return Monitor(glfwGetWindowMonitor(this->_handle));
}

boost::optional< Window > Window::getShare() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   auto itr = Window::_instances.find(this->_share);
   if (itr == Window::_instances.end()) {
      return boost::none;
   }

   return *itr->second;
}

std::string Window::getTitle() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return this->_title;
}

glm::ivec2 Window::getWindowPosition() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   int x, y;
   glfwGetWindowPos(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Window::getWindowSize() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   int x, y;
   glfwGetWindowSize(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

glm::ivec2 Window::getFrameBufferSize() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   int x, y;
   glfwGetFramebufferSize(this->_handle, &x, &y);
   return glm::ivec2(x, y);
}

bool Window::isVisible() const {
   return this->getAttribute(GLFW_VISIBLE);
}

bool Window::isFocused() const {
   return this->getAttribute(GLFW_FOCUSED);
}

bool Window::isMinimized() const {
   return this->getAttribute(GLFW_ICONIFIED);
}

bool Window::isResizable() const {
   return this->getAttribute(GLFW_RESIZABLE);
}

bool Window::isDecorated() const {
   return this->getAttribute(GLFW_DECORATED);
}

bool Window::shouldClose() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return glfwWindowShouldClose(this->_handle);
}

int Window::getAttribute(int a) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return glfwGetWindowAttrib(this->_handle, a);
}

int Window::getInputMode(int m) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return glfwGetInputMode(this->_handle, m);
}

void* Window::getUserPointer() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   return glfwGetWindowUserPointer(this->_handle);
}

int Window::getKey(int key) const {
   return glfwGetKey(this->_handle, key);
}

int Window::getMouseButton(int button) const {
   return glfwGetMouseButton(this->_handle, button);
}

glm::vec2 Window::getMousePosition() const {
   double x, y;
   glfwGetCursorPos(this->_handle, &x, &y);
   return glm::vec2(x, y);
}

bool Window::isDestroyed() const {
   return this->_destroyed;
}

void Window::setTitle(const std::string& t) {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetWindowTitle(this->_handle, t.data());
   // There is no glfwGetWindowTitle so we have to hold on to the new value.
   this->_title = t;
}

void Window::setWindowPosition(const glm::ivec2& p) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetWindowPos(this->_handle, p.x, p.y);
}

void Window::setWindowSize(const glm::ivec2& s) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetWindowSize(this->_handle, s.x, s.y);
}

void Window::setVisible(bool v) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (v) {
      glfwShowWindow(this->_handle);
   } else {
      glfwHideWindow(this->_handle);
   }
}

void Window::setMinimized(bool m) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (m) {
      glfwIconifyWindow(this->_handle);
   } else {
      glfwRestoreWindow(this->_handle);
   }
}

void Window::setShouldClose(bool c) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetWindowShouldClose(this->_handle, c ? GL_TRUE : GL_FALSE);
}

void Window::setInputMode(int m, int v) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetInputMode(this->_handle, m, v);
}

void Window::setUserPointer(void* p) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetWindowUserPointer(this->_handle, p);
}

void Window::setMousePosition(const glm::vec2& p) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSetCursorPos(this->_handle, p.x, p.y);
}

////////////////////////////////////////////////////////////////////////////////
// Callback management.
////////////////////////////////////////////////////////////////////////////////

boost::optional< Window::windowPositionCallback >
 Window::getWindowPositionCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_windowPositionCb == nullptr) {
      return boost::none;
   }

   return this->_windowPositionCb;
}

boost::optional< Window::windowSizeCallback >
 Window::getWindowSizeCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_windowSizeCb == nullptr) {
      return boost::none;
   }

   return this->_windowSizeCb;
}

boost::optional< Window::closeCallback > Window::getCloseCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_closeCb == nullptr) {
      return boost::none;
   }

   return this->_closeCb;
}

boost::optional< Window::refreshCallback > Window::getRefreshCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_refreshCb == nullptr) {
      return boost::none;
   }

   return this->_refreshCb;
}

boost::optional< Window::focusCallback > Window::getFocusCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_focusCb == nullptr) {
      return boost::none;
   }

   return this->_focusCb;
}

boost::optional< Window::minimizeCallback >
 Window::getMinimizeCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_minimizeCb == nullptr) {
      return boost::none;
   }

   return this->_minimizeCb;
}

boost::optional< Window::frameBufferSizeCallback >
 Window::getFrameBufferCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_frameBufferSizeCb == nullptr) {
      return boost::none;
   }

   return this->_frameBufferSizeCb;
}

boost::optional< Window::keyCallback > Window::getKeyCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_keyCb == nullptr) {
      return boost::none;
   }

   return this->_keyCb;
}

boost::optional< Window::charCallback > Window::getCharCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_charCb == nullptr) {
      return boost::none;
   }

   return this->_charCb;
}

boost::optional< Window::mouseButtonCallback >
 Window::getMouseButtonCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_mouseButtonCb == nullptr) {
      return boost::none;
   }

   return this->_mouseButtonCb;
}

boost::optional< Window::mousePositionCallback >
 Window::getMousePositionCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_mousePositionCb == nullptr) {
      return boost::none;
   }

   return this->_mousePositionCb;
}

boost::optional< Window::mouseEnterCallback >
 Window::getMouseEnterCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_mouseEnterCb == nullptr) {
      return boost::none;
   }

   return this->_mouseEnterCb;
}

boost::optional< Window::mouseScrollCallback >
 Window::getMouseScrollCallback() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (this->_mouseScrollCb == nullptr) {
      return boost::none;
   }

   return this->_mouseScrollCb;
}

boost::optional< Window::windowPositionCallback >
 Window::removeWindowPositionCallback() {
   return this->setWindowPositionCallback(nullptr);
}

boost::optional< Window::windowSizeCallback >
 Window::removeWindowSizeCallback() {
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

boost::optional< Window::frameBufferSizeCallback >
 Window::removeFrameBufferCallback() {
   return this->setFrameBufferSizeCallback(nullptr);
}

boost::optional< Window::keyCallback > Window::removeKeyCallback() {
   return this->setKeyCallback(nullptr);
}

boost::optional< Window::charCallback > Window::removeCharCallback() {
   return this->setCharCallback(nullptr);
}

boost::optional< Window::mouseButtonCallback >
 Window::removeMouseButtonCallback() {
   return this->setMouseButtonCallback(nullptr);
}

boost::optional< Window::mousePositionCallback >
 Window::removeMousePositionCallback() {
   return this->setMousePositionCallback(nullptr);
}

boost::optional< Window::mouseEnterCallback >
 Window::removeMouseEnterCallback() {
   return this->setMouseEnterCallback(nullptr);
}

boost::optional< Window::mouseScrollCallback >
 Window::removeMouseScrollCallback() {
   return this->setMouseScrollCallback(nullptr);
}

boost::optional< Window::windowPositionCallback >
 Window::setWindowPositionCallback(Window::windowPositionCallback callback) {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   Window::windowPositionCallback oldCallback = this->_windowPositionCb;
   this->_windowPositionCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::windowSizeCallback >
 Window::setWindowSizeCallback(Window::windowSizeCallback callback) {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
   }

   Window::frameBufferSizeCallback oldCallback = this->_frameBufferSizeCb;
   this->_frameBufferSizeCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Window::keyCallback >
 Window::setKeyCallback(keyCallback callback) {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   Window::keyCallback oldCallback = this->_keyCb;
   this->_keyCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

boost::optional< Window::charCallback >
 Window::setCharCallback(charCallback callback) {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   Window::charCallback oldCallback = this->_charCb;
   this->_charCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

boost::optional< Window::mouseButtonCallback >
 Window::setMouseButtonCallback(mouseButtonCallback callback) {
   Window::mouseButtonCallback oldCallback = this->_mouseButtonCb;
   this->_mouseButtonCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

boost::optional< Window::mousePositionCallback >
 Window::setMousePositionCallback(mousePositionCallback callback) {
   Window::mousePositionCallback oldCallback = this->_mousePositionCb;
   this->_mousePositionCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

boost::optional< Window::mouseEnterCallback >
 Window::setMouseEnterCallback(mouseEnterCallback callback) {
   Window::mouseEnterCallback oldCallback = this->_mouseEnterCb;
   this->_mouseEnterCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

boost::optional< Window::mouseScrollCallback >
 Window::setMouseScrollCallback(mouseScrollCallback callback) {
   Window::mouseScrollCallback oldCallback = this->_mouseScrollCb;
   this->_mouseScrollCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   }

   return oldCallback;
}

/* static */ void Window::windowPositionCallbackAdapter(GLFWwindow* handle,
 int x, int y) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (window->_windowPositionCb == nullptr) {
      return;
   }

   window->_windowPositionCb(*window, glm::ivec2(x, y));
}

/* static */ void Window::windowSizeCallbackAdapter(GLFWwindow* handle, int w,
 int h) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::DestroyedWindow();
   }

   if (window->_windowSizeCb == nullptr) {
      return;
   }

   window->_windowSizeCb(*window, glm::ivec2(w, h));
}

/* static */ void Window::frameBufferSizeCallbackAdapter(GLFWwindow* handle,
 int w, int h) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;

   if (window->_destroyed) {
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
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
      throw Window::DestroyedWindow();
   }

   if (window->_closeCb == nullptr) {
      return;
   }

   window->_closeCb(*window);
}

/* static */ void Window::keyCallbackAdapter(GLFWwindow* handle, int key,
 int scancode, int action, int mods) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_keyCb == nullptr) {
      return;
   }

   window->_keyCb(*window, key, scancode, action, mods);
}

/* static */ void Window::charCallbackAdapter(GLFWwindow* handle,
 unsigned int character) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_charCb == nullptr) {
      return;
   }

   window->_charCb(*window, character);
}

/* static */ void Window::mouseButtonCallbackAdapter(GLFWwindow* handle,
 int button, int action, int mods) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_mouseButtonCb == nullptr) {
      return;
   }

   window->_mouseButtonCb(*window, button, action, mods);
}

/* static */ void Window::mousePositionCallbackAdapter(GLFWwindow* handle,
 double x, double y) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_mousePositionCb == nullptr) {
      return;
   }

   window->_mousePositionCb(*window, glm::vec2(x, y));
}

/* static */ void Window::mouseEnterCallbackAdapter(GLFWwindow* handle,
 int entered) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_mouseEnterCb == nullptr) {
      return;
   }

   window->_mouseEnterCb(*window, entered ? GL_TRUE : GL_FALSE);
}

/* static */ void Window::mouseScrollCallbackAdapter(GLFWwindow* handle,
 double x, double y) {
   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return;
   }

   auto window = itr->second;
   if (window->_mouseScrollCb == nullptr) {
      return;
   }

   window->_mouseScrollCb(*window, glm::vec2(x, y));
}
////////////////////////////////////////////////////////////////////////////////
// GLFW callthroughs.
////////////////////////////////////////////////////////////////////////////////

void Window::swapBuffers() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSwapBuffers(this->_handle);
}

void Window::swapInterval(unsigned int interval) const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwSwapInterval(interval);
}

void Window::makeContextCurrent() const {
   if (this->_destroyed) {
      throw Window::DestroyedWindow();
   }

   glfwMakeContextCurrent(this->_handle);
}

void Window::destroy() {
   if (this->_destroyed) {
      return;
   }

   Window::_instances.erase(this->_handle);

   glfwDestroyWindow(this->_handle);
   this->_destroyed = true;
}

/* static */ void Window::setHint(int target, int hint) {
   glfwWindowHint(target, hint);
}

/* static */ boost::optional< Window > Window::getCurrentContext() {
   auto handle = glfwGetCurrentContext();
   if (handle == nullptr) {
      return boost::none;
   }

   auto itr = Window::_instances.find(handle);
   if (itr == Window::_instances.end()) {
      return boost::none;
   }

   return *itr->second;
}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ std::map< GLFWwindow*, Window* > Window::_instances;
