#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <boost/optional/optional.hpp>
#include <crash/window/glfw_adapter.hpp>

struct GLFWwindow;

namespace crash {
namespace window {

class Window;
typedef std::shared_ptr< window::Window > WindowPtr;

class Monitor;

class Window : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Type definitions.
   /////////////////////////////////////////////////////////////////////////////

   typedef void (*windowPositionCallback)(const Window&, const glm::ivec2&);
   typedef void (*windowSizeCallback)(const Window&, const glm::ivec2&);
   typedef void (*frameBufferSizeCallback)(const Window&, const glm::ivec2&);
   typedef void (*focusCallback)(const Window&, int);
   typedef void (*minimizeCallback)(const Window&, int);
   typedef void (*refreshCallback)(const Window&);
   typedef void (*closeCallback)(Window&);
   typedef void (*keyCallback)(const Window&, int, int, int, int);
   typedef void (*charCallback)(const Window&, unsigned int);
   typedef void (*mouseButtonCallback)(const Window&, int, int, int);
   typedef void (*mousePositionCallback)(const Window&, const glm::vec2&);
   typedef void (*mouseEnterCallback)(const Window&, bool);
   typedef void (*mouseScrollCallback)(const Window&, const glm::vec2&);

   static struct DestroyedWindow {} _destroyedWindow;

   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Window(const Window& window);
   Window(const glm::ivec2& size, const std::string& title,
    boost::optional< Monitor > monitor, boost::optional< Window > share);
   virtual ~Window();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   GLFWwindow* getHandle() const;
   boost::optional< Monitor > getMonitor() const;
   boost::optional< Window > getShare() const;

   std::string getTitle() const;
   glm::ivec2 getWindowPosition() const;
   glm::ivec2 getWindowSize() const;
   glm::ivec2 getFrameBufferSize() const;
   bool isVisible() const;
   bool isFocused() const;
   bool isMinimized() const;
   bool isResizable() const;
   bool isDecorated() const;
   bool shouldClose() const;
   int getAttribute(int a) const;
   int getInputMode(int m) const;
   void* getUserPointer() const;
   int getKey(int key) const;
   int getMouseButton(int button) const;
   glm::vec2 getMousePosition() const;

   bool isDestroyed() const;

   void setTitle(const std::string& t);
   void setWindowPosition(const glm::ivec2& p) const;
   void setWindowSize(const glm::ivec2& s) const;
   void setVisible(bool v) const;
   void setMinimized(bool m) const;
   void setShouldClose(bool c) const;
   void setInputMode(int m, int v) const;
   void setUserPointer(void* p) const;
   void setMousePosition(const glm::vec2& p) const;

   /////////////////////////////////////////////////////////////////////////////
   // Callback management.
   /////////////////////////////////////////////////////////////////////////////

   boost::optional< windowPositionCallback > getWindowPositionCallback() const;
   boost::optional< windowSizeCallback > getWindowSizeCallback() const;
   boost::optional< closeCallback > getCloseCallback() const;
   boost::optional< refreshCallback > getRefreshCallback() const;
   boost::optional< focusCallback > getFocusCallback() const;
   boost::optional< minimizeCallback > getMinimizeCallback() const;
   boost::optional< frameBufferSizeCallback > getFrameBufferCallback() const;
   boost::optional< keyCallback > getKeyCallback() const;
   boost::optional< charCallback > getCharCallback() const;
   boost::optional< mouseButtonCallback > getMouseButtonCallback() const;
   boost::optional< mousePositionCallback > getMousePositionCallback() const;
   boost::optional< mouseEnterCallback > getMouseEnterCallback() const;
   boost::optional< mouseScrollCallback > getMouseScrollCallback() const;

   boost::optional< windowPositionCallback > removeWindowPositionCallback();
   boost::optional< windowSizeCallback > removeWindowSizeCallback();
   boost::optional< closeCallback > removeCloseCallback();
   boost::optional< refreshCallback > removeRefreshCallback();
   boost::optional< focusCallback > removeFocusCallback();
   boost::optional< minimizeCallback > removeMinimizeCallback();
   boost::optional< frameBufferSizeCallback > removeFrameBufferCallback();
   boost::optional< keyCallback > removeKeyCallback();
   boost::optional< charCallback > removeCharCallback();
   boost::optional< mouseButtonCallback > removeMouseButtonCallback();
   boost::optional< mousePositionCallback > removeMousePositionCallback();
   boost::optional< mouseEnterCallback > removeMouseEnterCallback();
   boost::optional< mouseScrollCallback > removeMouseScrollCallback();

   boost::optional< windowPositionCallback >
    setWindowPositionCallback(windowPositionCallback callback);
   boost::optional< windowSizeCallback >
    setWindowSizeCallback(windowSizeCallback callback);
   boost::optional< closeCallback > setCloseCallback(closeCallback callback);
   boost::optional< refreshCallback >
    setRefreshCallback(refreshCallback callback);
   boost::optional< focusCallback > setFocusCallback(focusCallback callback);
   boost::optional< minimizeCallback >
    setMinimizeCallback(minimizeCallback callback);
   boost::optional< frameBufferSizeCallback >
    setFrameBufferSizeCallback(frameBufferSizeCallback callback);
   boost::optional< keyCallback > setKeyCallback(keyCallback callback);
   boost::optional< charCallback > setCharCallback(charCallback callback);
   boost::optional< mouseButtonCallback >
    setMouseButtonCallback(mouseButtonCallback callback);
   boost::optional< mousePositionCallback >
    setMousePositionCallback(mousePositionCallback callback);
   boost::optional< mouseEnterCallback >
    setMouseEnterCallback(mouseEnterCallback callback);
   boost::optional< mouseScrollCallback >
    setMouseScrollCallback(mouseScrollCallback callback);

   static void windowPositionCallbackAdapter(GLFWwindow* handle, int x, int y);
   static void windowSizeCallbackAdapter(GLFWwindow* handle, int w, int h);
   static void closeCallbackAdapter(GLFWwindow* handle);
   static void refreshCallbackAdapter(GLFWwindow* handle);
   static void focusCallbackAdapter(GLFWwindow* handle, int f);
   static void minimizeCallbackAdapter(GLFWwindow* handle, int m);
   static void frameBufferSizeCallbackAdapter(GLFWwindow* handle, int w, int h);
   static void keyCallbackAdapter(GLFWwindow* handle, int key, int scancode,
    int action, int mods);
   static void charCallbackAdapter(GLFWwindow* handle, unsigned int character);
   static void mouseButtonCallbackAdapter(GLFWwindow* handle, int button,
    int action, int mods);
   static void mousePositionCallbackAdapter(GLFWwindow* handle, double x,
    double y);
   static void mouseEnterCallbackAdapter(GLFWwindow* handle, int entered);
   static void mouseScrollCallbackAdapter(GLFWwindow* handle, double x,
    double y);

   /////////////////////////////////////////////////////////////////////////////
   // GLFW callthroughs.
   /////////////////////////////////////////////////////////////////////////////

   void swapBuffers() const;
   void swapInterval(unsigned int interval) const;
   void makeContextCurrent() const;
   void destroy();

   static void setHint(int target, int hint);

   static boost::optional< Window > getCurrentContext();

private:
   bool _destroyed;
   GLFWwindow* _handle;
   GLFWwindow* _share;
   std::string _title;

   windowPositionCallback _windowPositionCb;
   windowSizeCallback _windowSizeCb;
   closeCallback _closeCb;
   refreshCallback _refreshCb;
   focusCallback _focusCb;
   minimizeCallback _minimizeCb;
   frameBufferSizeCallback _frameBufferSizeCb;
   keyCallback _keyCb;
   charCallback _charCb;
   mouseButtonCallback _mouseButtonCb;
   mousePositionCallback _mousePositionCb;
   mouseEnterCallback _mouseEnterCb;
   mouseScrollCallback _mouseScrollCb;

   static std::map< GLFWwindow*, Window* > _instances;
};

} // namespace window
} // namespace crash
