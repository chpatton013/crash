#include <cmath>
#include <crash/common/arithmetic.hpp>

using namespace crash::common;

glm::quat crash::common::axisAngleToQuat(const glm::vec4& aa) {
   float c = std::cos(aa.w * 0.5f);
   float s = std::sin(aa.w * 0.5f);
   return glm::normalize(glm::quat(c, glm::normalize(glm::vec3(aa)) * s));
}
