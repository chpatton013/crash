#pragma once

#include <glm/glm.hpp>
#include <crash/window/glfw_adapter.hpp>

struct GLFWwindow;

namespace crash {
namespace window {

class Window;

class Mouse : public GlfwAdapter {
public:
   Mouse(const Mouse& mouse);
   Mouse(const Window& window);

   const Window& getWindow() const;

   int getButton(int button) const;

   glm::vec2 getPosition() const;
   void setPosition(const glm::vec2& position) const;

private:
   const Window& _window;
};

} // namespace window
} // namespace crash
