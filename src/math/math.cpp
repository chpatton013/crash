#include <crash/math/math.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/constants/constants.hpp>

using namespace crash::math;

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
