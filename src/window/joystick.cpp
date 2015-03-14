#include <GLFW/glfw3.h>
#include <crash/window/joystick.hpp>

using namespace crash::window;

Joystick::Joystick(const Joystick& joystick) :
   Joystick(joystick._handle)
{}

Joystick::Joystick(int handle) :
   _handle(handle)
{}

int Joystick::getHandle() const {
   return this->_handle;
}

bool Joystick::isPresent() const {
   return glfwJoystickPresent(this->_handle) == GL_TRUE;
}

boost::optional< std::vector< float > > Joystick::getAxes() const {
   int count = -1;
   const float* axes = glfwGetJoystickAxes(this->_handle, &count);

   if (axes == nullptr || count <= 0) {
      return boost::none;
   }

   std::vector< float > axesVector;
   for (int ndx = 0; ndx < count; ++ndx) {
      axesVector.push_back(axes[ndx]);
   }
   return axesVector;
}

boost::optional< std::vector< unsigned char > > Joystick::getButtons() const {
   int count = -1;
   const unsigned char* buttons = glfwGetJoystickButtons(this->_handle, &count);

   if (buttons == nullptr || count <= 0) {
      return boost::none;
   }

   std::vector< unsigned char > buttonsVector;
   for (int ndx = 0; ndx < count; ++ndx) {
      buttonsVector.push_back(buttons[ndx]);
   }
   return buttonsVector;
}

boost::optional< std::string > Joystick::getName() const {
   const char* name = glfwGetJoystickName(this->_handle);
   if (name == nullptr) {
      return boost::none;
   }

   return std::string(name);
}

/* static */ std::vector< Joystick > Joystick::getAllJoysticks() {
   std::vector< Joystick > joysticks;
   for (int ndx = 0; ndx < GLFW_JOYSTICK_LAST; ++ndx) {
      joysticks.push_back(Joystick(ndx));
   }
   return joysticks;
}

/* static */ std::vector< Joystick > Joystick::getConnectedJoysticks() {
   std::vector< Joystick > joysticks;
   for (int ndx = 0; ndx < GLFW_JOYSTICK_LAST; ++ndx) {
      if (glfwJoystickPresent(ndx) == GL_TRUE) {
         joysticks.push_back(Joystick(ndx));
      }
   }
   return joysticks;
}
