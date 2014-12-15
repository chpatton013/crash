#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <boost/optional/optional.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <crash/window/glfw_adapter.hpp>

namespace crash {
namespace window {

class Monitor;

void print(const Monitor& monitor);

void print(const Monitor& monitor, bool newline);

void print(const GLFWvidmode* mode);

void print(const GLFWvidmode* mode, bool newline);

void print(const GLFWgammaramp* gamma);

void print(const GLFWgammaramp* gamma, bool newline);

class Monitor : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Types.
   /////////////////////////////////////////////////////////////////////////////

   typedef void (*monitorCallback)(const Monitor&, int);

   /////////////////////////////////////////////////////////////////////////////
   // Getters.
   /////////////////////////////////////////////////////////////////////////////

   GLFWmonitor* handle() const;
   std::string name() const;
   glm::ivec2 position() const;
   glm::ivec2 size() const;
   std::vector< const GLFWvidmode* > availableModes() const;
   const GLFWvidmode* mode() const;
   const GLFWgammaramp* gamma() const;

   /////////////////////////////////////////////////////////////////////////////
   // Setters.
   /////////////////////////////////////////////////////////////////////////////

   void gamma(float gammaExponent) const;
   void gamma(const GLFWgammaramp* gammaRamp) const;

   /////////////////////////////////////////////////////////////////////////////
   // Static constructors.
   /////////////////////////////////////////////////////////////////////////////

   static boost::optional< Monitor* > factory();
   static boost::optional< Monitor* > factory(GLFWmonitor* handle);
   static bool release(Monitor* monitor);

   /////////////////////////////////////////////////////////////////////////////
   // Static getters.
   /////////////////////////////////////////////////////////////////////////////

   static boost::optional< GLFWmonitor* > primaryHandle();
   static boost::optional< std::vector< GLFWmonitor* > > availableHandles();

   static boost::optional< Monitor* > primaryMonitor();
   static boost::optional< std::vector< Monitor* > > availableMonitors();

   /////////////////////////////////////////////////////////////////////////////
   // Static initializers.
   /////////////////////////////////////////////////////////////////////////////

   static void initialize();
   static void teardown();

   /////////////////////////////////////////////////////////////////////////////
   // Callback management.
   /////////////////////////////////////////////////////////////////////////////

   static boost::optional< monitorCallback > getMonitorCallback();
   static boost::optional< monitorCallback > removeMonitorCallback();
   static boost::optional< monitorCallback >
    setMonitorCallback(monitorCallback callback);
   static void monitorCallbackAdapter(GLFWmonitor* handle, int state);

private:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Monitor();
   Monitor(GLFWmonitor* handle);
   virtual ~Monitor();

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   GLFWmonitor* _handle;

   /////////////////////////////////////////////////////////////////////////////
   // Static members.
   /////////////////////////////////////////////////////////////////////////////

   static monitorCallback _monitorCb;
   static std::unordered_map< GLFWmonitor*, Monitor* > _instances;

   static bool _initialized;

   static struct initializer {
      initializer();
   } _initializer;
};

} // namespace window
} // namespace crash
