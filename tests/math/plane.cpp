#include <crash/math/plane.hpp>
#include <catch.hpp>

#include <cmath>
#include <glm/glm.hpp>
#include <crash/util/util.hpp>

using namespace crash::math;

TEST_CASE("math/plane/fromPoints") {
   glm::vec3 a = glm::vec3(0.0f);
   glm::vec3 b = glm::vec3(1.0f);
   glm::vec3 c = glm::vec3(1.0f, 0.0f, 1.0f);

   Plane p1 = Plane::fromPoints(a, b, c);
   Plane p2 = Plane::fromPoints(c, b, a);

   glm::vec3 normal = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));

   REQUIRE(p1.normal == normal);
   REQUIRE(p2.normal == normal * -1.0f);
}

TEST_CASE("math/plane/distance") {
   Plane xyPlane = Plane(zAxis);
   Plane xzPlane = Plane(yAxis);
   Plane yzPlane = Plane(xAxis);

   glm::vec3 xPoint = glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 yPoint = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 zPoint = glm::vec3(0.0f, 0.0f, 1.0f);
   glm::vec3 xyPoint = glm::vec3(1.0f, 1.0f, 0.0f);
   glm::vec3 xzPoint = glm::vec3(1.0f, 0.0f, 1.0f);
   glm::vec3 yzPoint = glm::vec3(0.0f, 1.0f, 1.0f);
   glm::vec3 xyzPoint = glm::vec3(1.0f, 1.0f, 1.0f);

   REQUIRE(xyPlane.distance(origin) == 0.0f);
   REQUIRE(xyPlane.distance(xPoint) == 0.0f);
   REQUIRE(xyPlane.distance(yPoint) == 0.0f);
   REQUIRE(xyPlane.distance(zPoint) == 1.0f);
   REQUIRE(xyPlane.distance(xyPoint) == 0.0f);
   REQUIRE(xyPlane.distance(xzPoint) == 1.0f);
   REQUIRE(xyPlane.distance(yzPoint) == 1.0f);
   REQUIRE(xyPlane.distance(xyzPoint) == 1.0f);

   REQUIRE(xzPlane.distance(origin) == 0.0f);
   REQUIRE(xzPlane.distance(xPoint) == 0.0f);
   REQUIRE(xzPlane.distance(yPoint) == 1.0f);
   REQUIRE(xzPlane.distance(zPoint) == 0.0f);
   REQUIRE(xzPlane.distance(xyPoint) == 1.0f);
   REQUIRE(xzPlane.distance(xzPoint) == 0.0f);
   REQUIRE(xzPlane.distance(yzPoint) == 1.0f);
   REQUIRE(xzPlane.distance(xyzPoint) == 1.0f);

   REQUIRE(yzPlane.distance(origin) == 0.0f);
   REQUIRE(yzPlane.distance(xPoint) == 1.0f);
   REQUIRE(yzPlane.distance(yPoint) == 0.0f);
   REQUIRE(yzPlane.distance(zPoint) == 0.0f);
   REQUIRE(yzPlane.distance(xyPoint) == 1.0f);
   REQUIRE(yzPlane.distance(xzPoint) == 1.0f);
   REQUIRE(yzPlane.distance(yzPoint) == 0.0f);
   REQUIRE(yzPlane.distance(xyzPoint) == 1.0f);
}
