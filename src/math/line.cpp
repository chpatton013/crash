#include <crash/math/line.hpp>

using namespace crash::math;

Line::Line(const Line& line) :
   Line(line.point, line.direction)
{}

Line::Line(const glm::vec3& point, const glm::vec3& direction) :
   point(point), direction(direction)
{}

/* static */ Line Line::fromPoints(const glm::vec3& a, const glm::vec3& b) {
   return Line(a, b - a);
}

/* static */ Line Line::fromPoints(const glm::vec4& a, const glm::vec4& b) {
   return Line::fromPoints(glm::vec3(a), glm::vec3(b));
}
