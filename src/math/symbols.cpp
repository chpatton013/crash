#include <boost/math/constants/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <crash/math/symbols.hpp>

const float crash::math::pi = boost::math::constants::pi< float >();
const glm::vec3 crash::math::ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 crash::math::UNIT_SIZE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 crash::math::X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 crash::math::Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 crash::math::Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 crash::math::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 crash::math::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 crash::math::RIGHT = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::quat crash::math::NO_ROTATION = glm::quat();
