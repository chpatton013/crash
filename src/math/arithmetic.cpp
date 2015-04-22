#include <cmath>
#include <crash/math/arithmetic.hpp>

using namespace crash::math;

glm::quat crash::math::axisAngleToQuat(const glm::vec4& aa) {
   float c = std::cos(aa.w * 0.5f);
   float s = std::sin(aa.w * 0.5f);
   return glm::normalize(glm::quat(c, glm::normalize(glm::vec3(aa)) * s));
}
