#pragma once

#include <array>
#include <vector>
#include <boost/optional/optional.hpp>
#include <GLFW/glfw3.h>
#include <crash/window/glfw_adapter.hpp>

namespace crash {
namespace window {

class Joystick : public GlfwAdapter {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Getters.
   /////////////////////////////////////////////////////////////////////////////

   int handle() const;
   bool present() const;
   boost::optional< std::vector< float > > axes() const;
   boost::optional< std::vector< unsigned char > > buttons() const;
   boost::optional< std::string > name() const;

   /////////////////////////////////////////////////////////////////////////////
   // Static constructors.
   /////////////////////////////////////////////////////////////////////////////

   static boost::optional< Joystick* > factory(int joy);
   static void release(Joystick* joystick);

   /////////////////////////////////////////////////////////////////////////////
   // Static initializers.
   /////////////////////////////////////////////////////////////////////////////

   static void initialize();
   static void teardown();

private:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Joystick(int handle);
   virtual ~Joystick();

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   int _handle;

   /////////////////////////////////////////////////////////////////////////////
   // Static members.
   /////////////////////////////////////////////////////////////////////////////

   static std::array< Joystick*, GLFW_JOYSTICK_LAST > _instances;

   static bool _initialized;

   static struct initializer {
      initializer();
   } _initializer;
};

} // namespace window
} // namespace crash
