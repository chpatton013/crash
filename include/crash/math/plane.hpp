#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

struct Plane {
   Plane(const Plane& plane);
   Plane(const glm::vec3& point, const glm::vec3& normal);

   glm::vec3 point;
   glm::vec3 normal;

   /**
    * Calculate the signed orthogonal distance from a point to the plane.
    *
    * :param target: The reference point.
    * :return:       The calculated distance between the target point and plane.
    */
   float distance(const glm::vec3& target) const;

   /**
    * Calculate the signed orthogonal distance from a point to the plane.
    *
    * :param target: The reference point.
    * :return:       The calculated distance between the target point and plane.
    */
   float distance(const glm::vec4& target) const;

   /**
    * Generate a Plane from the three given non-colinear points.
    *
    * :param a:   The first point.
    * :param b:   The second point.
    * :param c:   The third point.
    * :return:    The generated Plane instance which contains the three given
    *             points.
    */
   static Plane fromPoints(const glm::vec3& a, const glm::vec3& b,
    const glm::vec3& c);

   /**
    * Generate a Plane from the three given non-colinear points.
    *
    * :param a:   The first point.
    * :param b:   The second point.
    * :param c:   The third point.
    * :return:    The generated Plane instance which contains the three given
    *             points.
    */
   static Plane fromPoints(const glm::vec4& a, const glm::vec4& b,
    const glm::vec4& c);

   /**
    * Generate a Plane from a point and two direction vectors.
    *
    * :param a:   A point on the plane.
    * :param b:   The first direction vector.
    * :param c:   The second direction vector.
    * :return:    The generated Plane instance that passes through the given
    *             point and has a normal vector equal to the cross product
    *             of the two given direction vectors.
    */
   static Plane fromDirections(const glm::vec3& p, const glm::vec3& d1,
    const glm::vec3& d2);

   /**
    * Generate a Plane from a point and two direction vectors.
    *
    * :param a:   A point on the plane.
    * :param b:   The first direction vector.
    * :param c:   The second direction vector.
    * :return:    The generated Plane instance that passes through the given
    *             point and has a normal vector equal to the cross product
    *             of the two given direction vectors.
    */
   static Plane fromDirections(const glm::vec4& p, const glm::vec4& d1,
    const glm::vec4& d2);
};

} // namespace math
} // namespace crash
