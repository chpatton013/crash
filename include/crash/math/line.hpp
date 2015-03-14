#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

struct Line {
   Line(const Line& line);
   Line(const glm::vec3& point, const glm::vec3& direction);

   glm::vec3 point;
   glm::vec3 direction;

   /**
    * Generate a Line from the two given points.
    *
    * :param a:   The first point.
    * :param b:   The second point.
    * :return:    The generated Line instance which spans the two given points.
    */
   static Line fromPoints(const glm::vec3& a, const glm::vec3& b);

   /**
    * Generate a Line from the two given points.
    *
    * :param a:   The first point.
    * :param b:   The second point.
    * :return:    The generated Line instance which spans the two given points.
    */
   static Line fromPoints(const glm::vec4& a, const glm::vec4& b);
};

} // namespace math
} // namespace crash
