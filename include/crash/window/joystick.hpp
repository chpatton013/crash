#pragma once

#include <vector>
#include <boost/optional/optional.hpp>
#include <crash/window/glfw_adapter.hpp>

namespace crash {
namespace window {

class Joystick : public GlfwAdapter {
public:
   Joystick(const Joystick& joystick);
   Joystick(int handle);

   int getHandle() const;
   bool isPresent() const;
   boost::optional< std::vector< float > > getAxes() const;
   boost::optional< std::vector< unsigned char > > getButtons() const;
   boost::optional< std::string > getName() const;

   static std::vector< Joystick > getAllJoysticks();
   static std::vector< Joystick > getConnectedJoysticks();

private:
   int _handle;
};

} // namespace window
} // namespace crash
