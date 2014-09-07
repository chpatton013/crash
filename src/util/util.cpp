#include <crash/util/util.hpp>

#include <stdio.h>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

using namespace crash::util;

int crash::util::linearize_index(const glm::ivec3& index,
 const glm::ivec3& bounds) {
   return (index.z * bounds.x * bounds.y) + (index.y * bounds.x) + index.x;
}

glm::ivec3 crash::util::vectorize_index(int index, const glm::ivec3& bounds) {
   // x: index % bounds.x
   // y: (index / bounds.x) % bounds.y
   // z: (index / bounds.x) / bounds.y
   std::div_t x = std::div(index, bounds.x);
   std::div_t yz = std::div(x.quot, bounds.y);
   return glm::ivec3(x.rem, yz.rem, yz.quot);
}

void crash::util::print(float f) {
   printf("%.4f\n", f);
}

void crash::util::print(const glm::vec3& v) {
   printf("%.4f %.4f %.4f\n", v.x, v.y, v.z);
}

void crash::util::print(const glm::vec4& v) {
   printf("%.4f %.4f %.4f %.4f\n", v.x, v.y, v.z, v.w);
}

void crash::util::print(const glm::mat4& m) {
   const float* vals = glm::value_ptr(m);
   for (int ndx = 0; ndx < 4; ++ndx) {
      int row = ndx * 4;
      printf("%.4f %.4f %.4f %.4f\n", vals[row + 0], vals[row + 1],
       vals[row + 2], vals[row + 3]);
   }
}
