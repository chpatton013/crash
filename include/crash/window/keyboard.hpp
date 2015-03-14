#pragma once

#include <crash/window/glfw_adapter.hpp>

namespace crash {
namespace window {

class Window;

class Keyboard : public GlfwAdapter {
public:
   Keyboard(const Keyboard& keyboard);
   Keyboard(const Window& window);

   const Window& getWindow() const;
   int getKey(int key) const;

private:
   const Window& _window;
};

} // namespace window
} // namespace crash
