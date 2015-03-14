#include <GLFW/glfw3.h>
#include <crash/window/keyboard.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

Keyboard::Keyboard(const Keyboard& keyboard) :
   Keyboard(keyboard._window)
{}

Keyboard::Keyboard(const Window& window) :
   _window(window)
{}

const Window& Keyboard::getWindow() const {
   return this->_window;
}

int Keyboard::getKey(int key) const {
   return glfwGetKey(this->_window.getHandle(), key);
}
