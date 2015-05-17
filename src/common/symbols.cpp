#include <boost/math/constants/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <crash/common/symbols.hpp>

const float crash::common::pi = boost::math::constants::pi< float >();
const glm::vec3 crash::common::ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 crash::common::UNIT_SIZE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 crash::common::X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 crash::common::Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 crash::common::Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 crash::common::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 crash::common::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 crash::common::RIGHT = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::quat crash::common::NO_ROTATION = glm::quat();
