#pragma once

#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <boost/optional/optional.hpp>
#include <crash/window/glfw_adapter.hpp>

namespace crash {
namespace window {

class Monitor;

class Window : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Type definitions.
   /////////////////////////////////////////////////////////////////////////////

   typedef void (*positionCallback)(const Window&, int, int);
   typedef void (*windowSizeCallback)(const Window&, int, int);
   typedef void (*frameBufferSizeCallback)(const Window&, int, int);
   typedef void (*closeCallback)(const Window&);
   typedef void (*refreshCallback)(const Window&);
   typedef void (*focusCallback)(const Window&, int);
   typedef void (*minimizeCallback)(const Window&, int);

   static struct DestroyedWindow {} _destroyedWindow;

   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Window(const glm::ivec2& size, const std::string& title);
   Window(const glm::ivec2& size, const std::string& title,
    boost::optional< Monitor* > monitor);
   Window(const glm::ivec2& size, const std::string& title,
    boost::optional< Monitor* > monitor, boost::optional< Window* > share);
   virtual ~Window();

   /////////////////////////////////////////////////////////////////////////////
   // Getters.
   /////////////////////////////////////////////////////////////////////////////

   GLFWwindow* handle() const;
   boost::optional< Monitor* > monitor() const;

   std::string title() const;
   glm::ivec2 position() const;
   glm::ivec2 windowSize() const;
   glm::ivec2 frameBufferSize() const;
   bool visible() const;
   bool focused() const;
   bool minimized() const;
   bool resizable() const;
   bool decorated() const;
   bool shouldClose() const;
   int attribute(int a) const;
   int inputMode(int m) const;
   void* userPointer() const;

   bool destroyed() const;

   boost::optional< positionCallback > getPositionCallback() const;
   boost::optional< windowSizeCallback > getWindowSizeCallback() const;
   boost::optional< closeCallback > getCloseCallback() const;
   boost::optional< refreshCallback > getRefreshCallback() const;
   boost::optional< focusCallback > getFocusCallback() const;
   boost::optional< minimizeCallback > getMinimizeCallback() const;
   boost::optional< frameBufferSizeCallback > getFrameBufferCallback() const;

   /////////////////////////////////////////////////////////////////////////////
   // Setters.
   /////////////////////////////////////////////////////////////////////////////

   void title(const std::string& t);
   void position(const glm::ivec2& p) const;
   void windowSize(const glm::ivec2& s) const;
   void visible(bool v) const;
   void minimized(bool m) const;
   void shouldClose(bool c) const;
   void inputMode(int m, int v) const;
   void userPointer(void* p) const;

   boost::optional< positionCallback > removePositionCallback();
   boost::optional< windowSizeCallback > removeWindowSizeCallback();
   boost::optional< closeCallback > removeCloseCallback();
   boost::optional< refreshCallback > removeRefreshCallback();
   boost::optional< focusCallback > removeFocusCallback();
   boost::optional< minimizeCallback > removeMinimizeCallback();
   boost::optional< frameBufferSizeCallback > removeFrameBufferCallback();

   boost::optional< positionCallback > setPositionCallback(positionCallback callback);
   boost::optional< windowSizeCallback > setWindowSizeCallback(windowSizeCallback callback);
   boost::optional< closeCallback > setCloseCallback(closeCallback callback);
   boost::optional< refreshCallback > setRefreshCallback(refreshCallback callback);
   boost::optional< focusCallback > setFocusCallback(focusCallback callback);
   boost::optional< minimizeCallback > setMinimizeCallback(minimizeCallback callback);
   boost::optional< frameBufferSizeCallback > setFrameBufferSizeCallback(frameBufferSizeCallback callback);

   /////////////////////////////////////////////////////////////////////////////
   // GLFW callthroughs.
   /////////////////////////////////////////////////////////////////////////////

   void swapBuffers() const;
   void swapInterval(unsigned int interval) const;
   void makeContextCurrent() const;
   void destroy();

   /////////////////////////////////////////////////////////////////////////////
   // Static setters.
   /////////////////////////////////////////////////////////////////////////////

   static void setHint(int target, int hint);

   /////////////////////////////////////////////////////////////////////////////
   // Static GLFW callthroughs.
   /////////////////////////////////////////////////////////////////////////////

   static boost::optional< Window* > getCurrentContext();

   /////////////////////////////////////////////////////////////////////////////
   // Callback adapters.
   /////////////////////////////////////////////////////////////////////////////

   static void positionCallbackAdapter(GLFWwindow* handle, int x, int y);
   static void windowSizeCallbackAdapter(GLFWwindow* handle, int w, int h);
   static void closeCallbackAdapter(GLFWwindow* handle);
   static void refreshCallbackAdapter(GLFWwindow* handle);
   static void focusCallbackAdapter(GLFWwindow* handle, int f);
   static void minimizeCallbackAdapter(GLFWwindow* handle, int m);
   static void frameBufferSizeCallbackAdapter(GLFWwindow* handle, int w, int h);

private:
   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   bool _destroyed;
   GLFWwindow* _handle;
   std::string _title;

   positionCallback _positionCb;
   windowSizeCallback _windowSizeCb;
   closeCallback _closeCb;
   refreshCallback _refreshCb;
   focusCallback _focusCb;
   minimizeCallback _minimizeCb;
   frameBufferSizeCallback _frameBufferSizeCb;

   /////////////////////////////////////////////////////////////////////////////
   // Static members.
   /////////////////////////////////////////////////////////////////////////////

   static std::unordered_map< GLFWwindow*, Window* > _instances;
};

} // namespace window
} // namespace crash
