#include <crash/window/mouse.hpp>

using namespace crash::window;

////////////////////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////////////////////

const Window& Mouse::window() const { return this->_window; }

int Mouse::button(int button) const {
   return glfwGetMouseButton(this->_window.handle(), button);
}

glm::vec2 Mouse::position() const {
   double x, y;
   glfwGetCursorPos(this->_window.handle(), &x, &y);
   return glm::vec2(x, y);
}

boost::optional< Mouse::buttonCallback > Mouse::getButtonCallback() const {
   return this->_buttonCb;
}

boost::optional< Mouse::positionCallback > Mouse::getPositionCallback() const {
   return this->_positionCb;
}

boost::optional< Mouse::enterCallback > Mouse::getEnterCallback() const {
   return this->_enterCb;
}

boost::optional< Mouse::scrollCallback > Mouse::getScrollCallback() const {
   return this->_scrollCb;
}

////////////////////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////////////////////

void Mouse::position(const glm::vec2& p) const {
   glfwSetCursorPos(this->_window.handle(), p.x, p.y);
}

boost::optional< Mouse::buttonCallback > Mouse::removeButtonCallback() {
   return this->setButtonCallback(nullptr);
}

boost::optional< Mouse::positionCallback > Mouse::removePositionCallback() {
   return this->setPositionCallback(nullptr);
}

boost::optional< Mouse::enterCallback > Mouse::removeEnterCallback() {
   return this->setEnterCallback(nullptr);
}

boost::optional< Mouse::scrollCallback > Mouse::removeScrollCallback() {
   return this->setScrollCallback(nullptr);
}

boost::optional< Mouse::buttonCallback > Mouse::setButtonCallback(buttonCallback callback) {
   Mouse::buttonCallback oldCallback = this->_buttonCb;
   this->_buttonCb = callback;

   if (oldCallback == nullptr) {
      return boost::optional< Mouse::buttonCallback >();
   } else {
      return boost::optional< Mouse::buttonCallback >(oldCallback);
   }
}

boost::optional< Mouse::positionCallback > Mouse::setPositionCallback(positionCallback callback) {
   Mouse::positionCallback oldCallback = this->_positionCb;
   this->_positionCb = callback;

   if (oldCallback == nullptr) {
      return boost::optional< Mouse::positionCallback >();
   } else {
      return boost::optional< Mouse::positionCallback >(oldCallback);
   }
}

boost::optional< Mouse::enterCallback > Mouse::setEnterCallback(enterCallback callback) {
   Mouse::enterCallback oldCallback = this->_enterCb;
   this->_enterCb = callback;

   if (oldCallback == nullptr) {
      return boost::optional< Mouse::enterCallback >();
   } else {
      return boost::optional< Mouse::enterCallback >(oldCallback);
   }
}

boost::optional< Mouse::scrollCallback > Mouse::setScrollCallback(scrollCallback callback) {
   Mouse::scrollCallback oldCallback = this->_scrollCb;
   this->_scrollCb = callback;

   if (oldCallback == nullptr) {
      return boost::optional< Mouse::scrollCallback >();
   } else {
      return boost::optional< Mouse::scrollCallback >(oldCallback);
   }
}

////////////////////////////////////////////////////////////////////////////////
// Static constructors.
////////////////////////////////////////////////////////////////////////////////

/* static */ Mouse* Mouse::factory(const Window& window) {
   return Mouse::_instances[window.handle()] = (new Mouse(window));
}

/* static */ bool Mouse::release(Mouse* mouse) {
   return Mouse::release(mouse->window());
}

/* static */ bool Mouse::release(const Window& window) {
   auto itr = Mouse::_instances.find(window.handle());
   if (itr == Mouse::_instances.end()) {
      return false;
   }

   Mouse::_instances.erase(itr);
   return true;
}

////////////////////////////////////////////////////////////////////////////////
// Callback management.
////////////////////////////////////////////////////////////////////////////////

/* static */ void Mouse::buttonCallbackAdapter(GLFWwindow* handle, int button, int action, int mods) {
   auto itr = Mouse::_instances.find(handle);
   if (itr == Mouse::_instances.end()) {
      return;
   }

   auto mouse = itr->second;
   if (mouse->_buttonCb == nullptr) {
      return;
   }

   mouse->_buttonCb(*mouse, button, action, mods);
}

/* static */ void Mouse::positionCallbackAdapter(GLFWwindow* handle, double x, double y) {
   auto itr = Mouse::_instances.find(handle);
   if (itr == Mouse::_instances.end()) {
      return;
   }

   auto mouse = itr->second;
   if (mouse->_positionCb == nullptr) {
      return;
   }

   mouse->_positionCb(*mouse, glm::vec2(x, y));
}

/* static */ void Mouse::enterCallbackAdapter(GLFWwindow* handle, int entered) {
   auto itr = Mouse::_instances.find(handle);
   if (itr == Mouse::_instances.end()) {
      return;
   }

   auto mouse = itr->second;
   if (mouse->_enterCb == nullptr) {
      return;
   }

   mouse->_enterCb(*mouse, entered ? GL_TRUE : GL_FALSE);
}

/* static */ void Mouse::scrollCallbackAdapter(GLFWwindow* handle, double x, double y) {
   auto itr = Mouse::_instances.find(handle);
   if (itr == Mouse::_instances.end()) {
      return;
   }

   auto mouse = itr->second;
   if (mouse->_scrollCb == nullptr) {
      return;
   }

   mouse->_scrollCb(*mouse, glm::vec2(x, y));
}

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Mouse::Mouse(const Window& window) :
   _window(window), _buttonCb(nullptr), _positionCb(nullptr), _enterCb(nullptr), _scrollCb(nullptr)
{
   glfwSetMouseButtonCallback(window.handle(), Mouse::buttonCallbackAdapter);
   glfwSetCursorPosCallback(window.handle(), Mouse::positionCallbackAdapter);
   glfwSetCursorEnterCallback(window.handle(), Mouse::enterCallbackAdapter);
   glfwSetScrollCallback(window.handle(), Mouse::scrollCallbackAdapter);
}

/* virtual */ Mouse::~Mouse() {
   glfwSetMouseButtonCallback(this->_window.handle(), nullptr);
   glfwSetCursorPosCallback(this->_window.handle(), nullptr);
   glfwSetCursorEnterCallback(this->_window.handle(), nullptr);
   glfwSetScrollCallback(this->_window.handle(), nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ std::unordered_map< GLFWwindow*, Mouse* > Mouse::_instances;
