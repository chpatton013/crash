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
   const float* v = glm::value_ptr(m);
   printf(
    "%.4f %.4f %.4f %.4f\n"
    "%.4f %.4f %.4f %.4f\n"
    "%.4f %.4f %.4f %.4f\n"
    "%.4f %.4f %.4f %.4f\n",
    v[0],  v[1],  v[2],  v[3],
    v[4],  v[5],  v[6],  v[7],
    v[8],  v[9],  v[10], v[11],
    v[12], v[13], v[14], v[15]);
}
