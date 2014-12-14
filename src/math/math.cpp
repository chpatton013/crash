#include <cstdio>
#include <cstdlib>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/math/constants/constants.hpp>
#include <crash/math/math.hpp>

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

int crash::math::linearize_index(const glm::ivec2& index,
 const glm::ivec2& bounds) {
   return (index.y * bounds.x) + index.x;
}

int crash::math::linearize_index(const glm::ivec3& index,
 const glm::ivec3& bounds) {
   return (index.z * bounds.x * bounds.y) + (index.y * bounds.x) + index.x;
}

glm::ivec2 crash::math::vectorize_index(int index, const glm::ivec2& bounds) {
   // x: index % bounds.x
   // y: index / bounds.x
   std::div_t q = std::div(index, bounds.x);
   return glm::ivec2(q.rem, q.quot);
}

glm::ivec3 crash::math::vectorize_index(int index, const glm::ivec3& bounds) {
   // x: index % bounds.x
   // y: (index / bounds.x) % bounds.y
   // z: (index / bounds.x) / bounds.y
   std::div_t x = std::div(index, bounds.x);
   std::div_t yz = std::div(x.quot, bounds.y);
   return glm::ivec3(x.rem, yz.rem, yz.quot);
}

glm::vec4 crash::math::rotation(const glm::vec3& axis, float angle) {
   return glm::vec4(axis, angle);
}

glm::vec4 crash::math::rotation(const glm::vec4& axis, float angle) {
   return glm::vec4(glm::vec3(axis), angle);
}

glm::mat4 crash::math::transform(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size) {
   glm::mat4 tr = transform(position, orientation);
   glm::mat4 scale = glm::scale(size);

   return tr * scale;
}

glm::mat4 crash::math::transform(const glm::vec3& position,
 const glm::vec4& orientation) {
   glm::mat4 translate = glm::translate(position);
   glm::mat4 rotate = glm::rotate(orientation.w, glm::vec3(orientation));

   return translate * rotate;
}

float const crash::math::pi = boost::math::constants::pi< float >();
const glm::vec3 crash::math::origin = glm::vec3(0.0f);
const glm::vec3 crash::math::xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 crash::math::yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 crash::math::zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
