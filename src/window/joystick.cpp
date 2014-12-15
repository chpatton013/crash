#include <crash/window/joystick.hpp>

using namespace crash::window;

/////////////////////////////////////////////////////////////////////////////
// Getters.
/////////////////////////////////////////////////////////////////////////////

int Joystick::handle() const { return this->_handle; }

bool Joystick::present() const {
   return glfwJoystickPresent(this->_handle) == GL_TRUE;
}

boost::optional< std::vector< float > > Joystick::axes() const {
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

boost::optional< std::vector< unsigned char > > Joystick::buttons() const {
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

boost::optional< std::string > Joystick::name() const {
   const char* name = glfwGetJoystickName(this->_handle);
   if (name == nullptr) {
      return boost::none;
   }

   return std::string(name);
}

/////////////////////////////////////////////////////////////////////////////
// Static constructors.
/////////////////////////////////////////////////////////////////////////////

/* static */ boost::optional< Joystick* > Joystick::factory(int joy) {
   if (Joystick::_instances[joy] == nullptr) {
      Joystick::_instances[joy] = new Joystick(joy);
   }

   return Joystick::_instances[joy];
}

/* static */ void Joystick::release(Joystick* joystick) {
   delete Joystick::_instances[joystick->_handle];
}

/////////////////////////////////////////////////////////////////////////////
// Static initializers.
/////////////////////////////////////////////////////////////////////////////

/* static */ void Joystick::initialize() {
   for (int ndx = 0; ndx < GLFW_JOYSTICK_LAST; ++ndx) {
      Joystick::factory(ndx);
   }
}

/* static */ void Joystick::teardown() {
   for (int ndx = 0; ndx < GLFW_JOYSTICK_LAST; ++ndx) {
      Joystick::release(Joystick::_instances[ndx]);
   }
}

/////////////////////////////////////////////////////////////////////////////
// Constructors.
/////////////////////////////////////////////////////////////////////////////

Joystick::Joystick(int handle) :
   _handle(handle)
{}

/* virtual */ Joystick::~Joystick() {}

////////////////////////////////////////////////////////////////////////////////
// Static members.
////////////////////////////////////////////////////////////////////////////////

/* static */ std::array< Joystick*, GLFW_JOYSTICK_LAST > Joystick::_instances;

/* static */ bool Joystick::_initialized = false;

Joystick::initializer::initializer() {
   Joystick::initialize();
}

/* static */ Joystick::initializer Joystick::_initializer;
