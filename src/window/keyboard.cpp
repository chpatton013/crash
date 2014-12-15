#include <crash/window/keyboard.hpp>

using namespace crash::window;

////////////////////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////////////////////

const Window& Keyboard::window() const { return this->_window; }

int Keyboard::key(int k) const {
   return glfwGetKey(this->_window.handle(), k);
}

boost::optional< Keyboard::keyCallback > Keyboard::getKeyCallback() const {
   return this->_keyCb;
}

boost::optional< Keyboard::charCallback > Keyboard::getCharCallback() const {
   return this->_charCb;
}

////////////////////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////////////////////

boost::optional< Keyboard::keyCallback > Keyboard::removeKeyCallback() {
   return this->setKeyCallback(nullptr);
}

boost::optional< Keyboard::charCallback > Keyboard::removeCharCallback() {
   return this->setCharCallback(nullptr);
}

boost::optional< Keyboard::keyCallback > Keyboard::setKeyCallback(keyCallback callback) {
   Keyboard::keyCallback oldCallback = this->_keyCb;
   this->_keyCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

boost::optional< Keyboard::charCallback > Keyboard::setCharCallback(charCallback callback) {
   Keyboard::charCallback oldCallback = this->_charCb;
   this->_charCb = callback;

   if (oldCallback == nullptr) {
      return boost::none;
   } else {
      return oldCallback;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Static constructors.
////////////////////////////////////////////////////////////////////////////////

/* static */ Keyboard* Keyboard::factory(const Window& window) {
   return Keyboard::_instances[window.handle()] = (new Keyboard(window));
}

/* static */ bool Keyboard::release(Keyboard* keyboard) {
   return Keyboard::release(keyboard->window());
}

/* static */ bool Keyboard::release(const Window& window) {
   auto itr = Keyboard::_instances.find(window.handle());
   if (itr == Keyboard::_instances.end()) {
      return false;
   }

   Keyboard::_instances.erase(itr);
   return true;
}

////////////////////////////////////////////////////////////////////////////////
// Callback management.
////////////////////////////////////////////////////////////////////////////////

/* static */ void Keyboard::keyCallbackAdapter(GLFWwindow* handle, int key, int scancode, int action, int mods) {
   auto itr = Keyboard::_instances.find(handle);
   if (itr == Keyboard::_instances.end()) {
      return;
   }

   auto keyboard = itr->second;
   if (keyboard->_keyCb == nullptr) {
      return;
   }

   keyboard->_keyCb(*keyboard, key, scancode, action, mods);
}

/* static */ void Keyboard::charCallbackAdapter(GLFWwindow* handle, unsigned int character) {
   auto itr = Keyboard::_instances.find(handle);
   if (itr == Keyboard::_instances.end()) {
      return;
   }

   auto keyboard = itr->second;
   if (keyboard->_keyCb == nullptr) {
      return;
   }

   keyboard->_charCb(*keyboard, character);
}

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Keyboard::Keyboard(const Window& window) :
   _window(window), _keyCb(nullptr), _charCb(nullptr)
{
   glfwSetKeyCallback(window.handle(), Keyboard::keyCallbackAdapter);
   glfwSetCharCallback(window.handle(), Keyboard::charCallbackAdapter);

   Keyboard::_instances[this->_window.handle()] = this;
}

/* virtual */ Keyboard::~Keyboard() {
   if (!this->_window.destroyed()) {
      glfwSetKeyCallback(this->_window.handle(), nullptr);
      glfwSetCharCallback(this->_window.handle(), nullptr);
   }

   Keyboard::_instances.erase(this->_window.handle());
}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ std::unordered_map< GLFWwindow*, Keyboard* > Keyboard::_instances;
