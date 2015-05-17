#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <crash/math/util.hpp>

using namespace crash::math;

void crash::math::print(float f) {
   print(f, true);
}

void crash::math::print(float f, bool newline) {
   if (newline) {
      printf("%.4f\n", f);
   } else {
      printf("%.4f", f);
   }
}

void crash::math::print(const glm::vec3& v) {
   print(v, true);
}

void crash::math::print(const glm::vec3& v, bool newline) {
   if (newline) {
      printf("%.4f %.4f %.4f\n", v.x, v.y, v.z);
   } else {
      printf("%.4f %.4f %.4f", v.x, v.y, v.z);
   }
}

void crash::math::print(const glm::vec4& v) {
   print(v, true);
}

void crash::math::print(const glm::vec4& v, bool newline) {
   if (newline) {
      printf("%.4f %.4f %.4f %.4f\n", v.x, v.y, v.z, v.w);
   } else {
      printf("%.4f %.4f %.4f %.4f", v.x, v.y, v.z, v.w);
   }
}

void crash::math::print(const glm::ivec3& v) {
   print(v, true);
}

void crash::math::print(const glm::ivec3& v, bool newline) {
   if (newline) {
      printf("%d %d %d\n", v.x, v.y, v.z);
   } else {
      printf("%d %d %d", v.x, v.y, v.z);
   }
}

void crash::math::print(const glm::ivec4& v) {
   print(v, true);
}

void crash::math::print(const glm::ivec4& v, bool newline) {
   if (newline) {
      printf("%d %d %d %d\n", v.x, v.y, v.z, v.w);
   } else {
      printf("%d %d %d %d", v.x, v.y, v.z, v.w);
   }
}

void crash::math::print(const glm::quat& q) {
   print(q, true);
}

void crash::math::print(const glm::quat& q, bool newline) {
   if (newline) {
      printf("%.4f %.4f %.4f %.4f\n", q.w, q.x, q.y, q.z);
   } else {
      printf("%.4f %.4f %.4f %.4f", q.w, q.x, q.y, q.z);
   }
}

void crash::math::print(const glm::mat4& m) {
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
