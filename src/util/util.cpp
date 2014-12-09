#include <crash/util/util.hpp>

#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>

using namespace crash::util;

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
