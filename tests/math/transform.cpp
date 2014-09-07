#include <crash/math/math.hpp>
#include <catch.hpp>
#include <crash/util/util.hpp>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace crash::math;

static void testTransformation(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size,
 const glm::mat4& expectedMatrix, const glm::vec4& expectedPoint) {
   glm::mat4 transformMatrix = transform(position, orientation, size);
   REQUIRE(approxEqual(transformMatrix, expectedMatrix));

   glm::vec4 point = glm::vec4(1.0f);
   glm::vec4 transformPoint = transformMatrix * point;
   REQUIRE(approxEqual(expectedPoint, transformPoint));
}

TEST_CASE("math/transform/identity") {
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec4 orientation = glm::vec4(xAxis, 0.0f);
   glm::vec3 size = glm::vec3(1.0f);
   glm::mat4 identity;
   glm::vec4 point = glm::vec4(1.0f);

   testTransformation(position, orientation, size, identity, point);
}

TEST_CASE("math/transform/translate") {
   glm::vec3 position = glm::vec3(2.0f, 3.0f, 4.0f);
   glm::vec4 orientation = glm::vec4(xAxis, 0.0f);
   glm::vec3 size = glm::vec3(1.0f);

   float expectedArray[16] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      2.0f, 3.0f, 4.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(3.0f, 4.0f, 5.0f, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/rotate") {
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec4 orientation = glm::vec4(xAxis, pi / 4.0f);
   glm::vec3 size = glm::vec3(1.0f);

   float root2 = std::sqrt(2);
   float expectedArray[16] = {
      1.0f,          0.0f,         0.0f, 0.0f,
      0.0f,  0.5f * root2, 0.5f * root2, 0.0f,
      0.0f, -0.5f * root2, 0.5f * root2, 0.0f,
      0.0f,          0.0f,         0.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(1.0f, 0.0f, root2, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/scale") {
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec4 orientation = glm::vec4(xAxis, 0.0f);
   glm::vec3 size = glm::vec3(2.0f, 3.0f, 4.0f);

   glm::mat4 trans = transform(position, orientation, size);
   float expectedArray[16] = {
      2.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 3.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 4.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(2.0f, 3.0f, 4.0f, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/translate-rotate") {
   glm::vec3 position = glm::vec3(2.0f, 3.0f, 4.0f);
   glm::vec4 orientation = glm::vec4(xAxis, pi / 4.0f);
   glm::vec3 size = glm::vec3(1.0f);

   float root2 = std::sqrt(2);
   float invRoot2 = 1 / std::sqrt(2);
   float expectedArray[16] = {
      1.0f,      0.0f,     0.0f, 0.0f,
      0.0f,  invRoot2, invRoot2, 0.0f,
      0.0f, -invRoot2, invRoot2, 0.0f,
      2.0f,      3.0f,     4.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(3.0f, 3.0f, 4.0f + root2, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/translate-scale") {
   glm::vec3 position = glm::vec3(2.0f, 3.0f, 4.0f);
   glm::vec4 orientation = glm::vec4(xAxis, 0.0f);
   glm::vec3 size = glm::vec3(2.0f, 3.0f, 4.0f);

   float expectedArray[16] = {
      2.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 3.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 4.0f, 0.0f,
      2.0f, 3.0f, 4.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(4.0f, 6.0f, 8.0f, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/rotate-scale") {
   glm::vec3 position = glm::vec3(0.0f);
   glm::vec4 orientation = glm::vec4(xAxis, pi / 4.0f);
   glm::vec3 size = glm::vec3(2.0f, 3.0f, 4.0f);

   float root2 = std::sqrt(2);
   float invRoot2 = 1 / std::sqrt(2);
   float expectedArray[16] = {
      2.0f,          0.0f,         0.0f, 0.0f,
      0.0f,  1.5f * root2, 1.5f * root2, 0.0f,
      0.0f, -2.0f * root2, 2.0f * root2, 0.0f,
      0.0f,          0.0f,         0.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(2.0f, -invRoot2, 7.0f * invRoot2, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}

TEST_CASE("math/transform/translate-rotate-scale") {
   glm::vec3 position = glm::vec3(2.0f, 3.0f, 4.0f);
   glm::vec4 orientation = glm::vec4(xAxis, pi / 4.0f);
   glm::vec3 size = glm::vec3(2.0f, 3.0f, 4.0f);

   float root2 = std::sqrt(2);
   float invRoot2 = 1 / std::sqrt(2);
   float expectedArray[16] = {
      2.0f,             0.0f,            0.0f, 0.0f,
      0.0f,  3.0f * invRoot2, 3.0f * invRoot2, 0.0f,
      0.0f, 4.0f * -invRoot2, 4.0f * invRoot2, 0.0f,
      2.0f,             3.0f,            4.0f, 1.0f
   };
   glm::mat4 expectedMatrix = glm::make_mat4(expectedArray);
   glm::vec4 expectedPoint = glm::vec4(4.0f, 3.0f - invRoot2, 4.0f + 7.0f * invRoot2, 1.0f);

   testTransformation(position, orientation, size, expectedMatrix, expectedPoint);
}
