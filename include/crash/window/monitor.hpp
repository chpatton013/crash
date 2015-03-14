#pragma once

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include <glm/glm.hpp>
#include <crash/window/glfw_adapter.hpp>

struct GLFWgammaramp;
struct GLFWmonitor;
struct GLFWvidmode;

namespace crash {
namespace window {

class Monitor : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Types.
   /////////////////////////////////////////////////////////////////////////////

   typedef void (*monitorCallback)(const Monitor&, int);

   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Monitor();
   Monitor(const Monitor& monitor);
   Monitor(GLFWmonitor* handle);

   /////////////////////////////////////////////////////////////////////////////
   // GLFW callthroughs.
   /////////////////////////////////////////////////////////////////////////////

   GLFWmonitor* getHandle() const;
   std::string getName() const;
   glm::ivec2 getPosition() const;
   glm::ivec2 getSize() const;
   std::vector< const GLFWvidmode* > getAvailableModes() const;
   const GLFWvidmode* getMode() const;
   const GLFWgammaramp* getGamma() const;

   void setGammaExponent(float gammaExponent) const;
   void setGammaRamp(const GLFWgammaramp* gammaRamp) const;

   /////////////////////////////////////////////////////////////////////////////
   // Static accessors.
   /////////////////////////////////////////////////////////////////////////////

   static GLFWmonitor* getPrimaryHandle();
   static std::vector< GLFWmonitor* > getAvailableHandles();

   static boost::optional< Monitor > getPrimaryMonitor();
   static boost::optional< std::vector< Monitor > > getAvailableMonitors();

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
   static boost::optional< monitorCallback > setMonitorCallback(
    monitorCallback callback);
   static void monitorCallbackAdapter(GLFWmonitor* handle, int state);

private:
   GLFWmonitor* _handle;

   static monitorCallback _monitorCb;

   static bool _initialized;
   static struct initializer {
      initializer();
   } _initializer;
};

} // namespace window
} // namespace crash
