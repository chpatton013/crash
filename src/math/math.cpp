#include <crash/math/math.hpp>

#include <stdint.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/constants/constants.hpp>

using namespace crash::math;

float crash::math::fastInvSqrt(float x) {
   union inv_sqrt_t {
      int32_t i;
      float x;
   };

   union inv_sqrt_t u;
   // Interpret 'x' as an integer without making any modifications to the
   // binary value. Then apply magic by bitshifting and a little arithmetic.
   u.x = x;
   u.i = 0X5F3759DF - (u.i >> 1);
   // Apply Newton's Method to get a better approximation.
   return u.x * (1.5f - (x * 0.5f * u.x * u.x));
}

float crash::math::fastSqrt(float x) {
   return 1 / fastInvSqrt(x);
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
