#include <cmath>
#include <limits>
#include <random>
#include <crash/common/arithmetic.hpp>

using namespace crash::common;

int crash::common::rand_int() {
   return rand_int(std::numeric_limits< int >::max());
}

int crash::common::rand_int(int upper) {
   return rand_int(0, upper);
}

int crash::common::rand_int(int lower, int upper) {
   static std::random_device device;
   static std::default_random_engine engine(device());

   std::uniform_int_distribution< int > uniform_dist(lower, upper);
   return uniform_dist(engine);
}

float crash::common::unit_rand() {
   return rand_int() / (float)std::numeric_limits< int >::max();
}

float crash::common::rand_float() {
   return rand_float(std::numeric_limits< float >::max());
}

float crash::common::rand_float(float upper) {
   return rand_float(0, upper);
}

float crash::common::rand_float(float lower, float upper) {
   float difference = upper - lower;
   return lower + difference * unit_rand();
}

glm::quat crash::common::axisAngleToQuat(const glm::vec3& axis, float angle) {
   float c = std::cos(angle * 0.5f);
   float s = std::sin(angle * 0.5f);
   return glm::normalize(glm::quat(c, glm::normalize(axis) * s));
}

glm::quat crash::common::axisAngleToQuat(const glm::vec4& aa) {
   return axisAngleToQuat(glm::vec3(aa), aa.w);
}
