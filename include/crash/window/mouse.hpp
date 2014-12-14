#pragma once

#include <unordered_map>
#include <boost/optional/optional.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <crash/window/glfw_adapter.hpp>
#include <crash/window/window.hpp>

namespace crash {
namespace window {

class Mouse : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Type definitions.
   /////////////////////////////////////////////////////////////////////////////

   typedef void (*buttonCallback)(const Mouse&, int, int, int);
   typedef void (*positionCallback)(const Mouse&, const glm::vec2&);
   typedef void (*enterCallback)(const Mouse&, bool);
   typedef void (*scrollCallback)(const Mouse&, const glm::vec2&);

   /////////////////////////////////////////////////////////////////////////////
   // Getters.
   /////////////////////////////////////////////////////////////////////////////

   const Window& window() const;

   int button(int button) const;
   glm::vec2 position() const;

   boost::optional< buttonCallback > getButtonCallback() const;
   boost::optional< positionCallback > getPositionCallback() const;
   boost::optional< enterCallback > getEnterCallback() const;
   boost::optional< scrollCallback > getScrollCallback() const;

   /////////////////////////////////////////////////////////////////////////////
   // Setters.
   /////////////////////////////////////////////////////////////////////////////

   void position(const glm::vec2& p) const;

   boost::optional< buttonCallback > removeButtonCallback();
   boost::optional< positionCallback > removePositionCallback();
   boost::optional< enterCallback > removeEnterCallback();
   boost::optional< scrollCallback > removeScrollCallback();

   boost::optional< buttonCallback > setButtonCallback(buttonCallback callback);
   boost::optional< positionCallback > setPositionCallback(positionCallback callback);
   boost::optional< enterCallback > setEnterCallback(enterCallback callback);
   boost::optional< scrollCallback > setScrollCallback(scrollCallback callback);

   /////////////////////////////////////////////////////////////////////////////
   // Static constructors.
   /////////////////////////////////////////////////////////////////////////////

   static Mouse* factory(const Window& window);
   static bool release(Mouse* mouse);
   static bool release(const Window& window);

   /////////////////////////////////////////////////////////////////////////////
   // Callback management.
   /////////////////////////////////////////////////////////////////////////////

   static void buttonCallbackAdapter(GLFWwindow* handle, int button, int action, int mods);
   static void positionCallbackAdapter(GLFWwindow* handle, double x, double y);
   static void enterCallbackAdapter(GLFWwindow* handle, int entered);
   static void scrollCallbackAdapter(GLFWwindow* handle, double x, double y);

private:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Mouse(const Window& window);
   virtual ~Mouse();

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   const Window& _window;

   buttonCallback _buttonCb;
   positionCallback _positionCb;
   enterCallback _enterCb;
   scrollCallback _scrollCb;

   /////////////////////////////////////////////////////////////////////////////
   // Static members.
   /////////////////////////////////////////////////////////////////////////////

   static std::unordered_map< GLFWwindow*, Mouse* > _instances;
};

} // namespace window
} // namespace crash
