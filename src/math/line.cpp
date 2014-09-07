#include <crash/math/plane.hpp>
#include <crash/math/math.hpp>

using namespace crash::math;

Plane::Plane(const glm::vec3& normal) :
   point(math::origin), normal(normal)
{}

Plane::Plane(const glm::vec3& point, const glm::vec3& normal) :
   point(point), normal(normal)
{}

float Plane::distance(const glm::vec3& target) const {
   // Projection of this->normal onto (this->point - target).
   // Normally, the projection of two vectors requires a division:
   //    |proj(v,w)| = |v dot w| / |v|
   // But normal vectors in planes are expected to be normalized, so |v| = 1.
   return glm::dot(this->normal, target - this->point);
}

float Plane::distance(const glm::vec4& target) const {
   return this->distance(glm::vec3(target));
}

// static
Plane Plane::fromPoints(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
   glm::vec3 v = b - a;
   glm::vec3 w = c - b;

   return Plane(a, glm::normalize(glm::cross(w, v)));
}

// static
Plane Plane::fromDirections(const glm::vec3& p, const glm::vec3& d1, const glm::vec3& d2) {
   return Plane(p, glm::normalize(glm::cross(d1, d2)));
}
