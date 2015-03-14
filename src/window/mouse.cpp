#include <GLFW/glfw3.h>
#include <crash/window/mouse.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

Mouse::Mouse(const Mouse& mouse) :
   Mouse(mouse._window)
{}

Mouse::Mouse(const Window& window) :
   _window(window)
{}

const Window& Mouse::getWindow() const {
   return this->_window;
}

int Mouse::getButton(int button) const {
   return glfwGetMouseButton(this->_window.getHandle(), button);
}

glm::vec2 Mouse::getPosition() const {
   double x, y;
   glfwGetCursorPos(this->_window.getHandle(), &x, &y);
   return glm::vec2(x, y);
}

void Mouse::setPosition(const glm::vec2& position) const {
   glfwSetCursorPos(this->_window.getHandle(), position.x, position.y);
}
