#include <array>
#include <cmath>
#include <vector>
#include <crash/math/math.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/camera.hpp>
#include <crash/util/type.hpp>
#include <catch.hpp>

using namespace crash::space;

static void assertPointVisibility(Camera& camera,
 const std::vector< glm::vec3 >& inView,
 const std::vector< glm::vec3 >& outOfView) {
   for (auto point : inView) {
      REQUIRE(camera.isPointVisible(point));
   }
   for (auto point : outOfView) {
      REQUIRE(!camera.isPointVisible(point));
   }
}

static void assertCamera(const glm::vec3& position,
 const glm::vec4& orientation, float fov, float aspect) {
   float tanX = tan(fov * aspect * 0.5f);
   float tanY = tan(fov * 0.5f);
   glm::vec3 near = glm::vec3(tanX, tanY, 1.0f);
   glm::vec3 far = 10.0f * near;
   glm::vec3 mid = crash::math::average(near, far);
   glm::vec3 buffer = glm::vec3(tanX * 1.5f, tanY * 1.5f, 1.0f);

   Camera c = Camera(position, orientation, fov, aspect, near.z, far.z);

   std::vector< glm::vec3 > inView = {{
      // Behind near plane.
      glm::vec3(              0.0f,               0.0f, near.z + 1.0f),
      glm::vec3( near.x - buffer.x,  near.y - buffer.y, near.z + 1.0f),
      glm::vec3(-near.x + buffer.x,  near.y - buffer.y, near.z + 1.0f),
      glm::vec3( near.x - buffer.x, -near.y + buffer.y, near.z + 1.0f),
      glm::vec3(-near.x + buffer.x, -near.y + buffer.y, near.z + 1.0f),
      // Approximate mid-distance.
      glm::vec3(             0.0f,              0.0f, mid.z),
      glm::vec3(           near.x,            near.y, mid.z),
      glm::vec3(          -near.x,            near.y, mid.z),
      glm::vec3(           near.x,           -near.y, mid.z),
      glm::vec3(          -near.x,           -near.y, mid.z),
      glm::vec3( mid.x - buffer.x,  mid.y - buffer.y, mid.z),
      glm::vec3(-mid.x + buffer.x,  mid.y - buffer.y, mid.z),
      glm::vec3( mid.x - buffer.x, -mid.y + buffer.y, mid.z),
      glm::vec3(-mid.x + buffer.x, -mid.y + buffer.y, mid.z),
      // In front of far plane.
      glm::vec3(             0.0f,              0.0f, far.z - 1.0f),
      glm::vec3(            mid.x,             mid.y, far.z - 1.0f),
      glm::vec3(           -mid.x,             mid.y, far.z - 1.0f),
      glm::vec3(            mid.x,            -mid.y, far.z - 1.0f),
      glm::vec3(           -mid.x,            -mid.y, far.z - 1.0f),
      glm::vec3( far.x - buffer.x,  far.y - buffer.y, far.z - 1.0f),
      glm::vec3(-far.x + buffer.x,  far.y - buffer.y, far.z - 1.0f),
      glm::vec3( far.x - buffer.x, -far.y + buffer.y, far.z - 1.0f),
      glm::vec3(-far.x + buffer.x, -far.y + buffer.y, far.z - 1.0f),
   }};

   std::vector< glm::vec3 > outOfView = {{
      // Before camera.
      glm::vec3(0.0f, 0.0f, -1.0f),
      glm::vec3(0.0f, 0.0f,  0.0f),
      // Outside near plane.
      glm::vec3( near.x + buffer.x,  near.y + buffer.y, near.z),
      glm::vec3(              0.0f,  near.y + buffer.y, near.z),
      glm::vec3(-near.x - buffer.x,  near.y + buffer.y, near.z),
      glm::vec3( near.x + buffer.x,               0.0f, near.z),
      glm::vec3(-near.x - buffer.x,               0.0f, near.z),
      glm::vec3( near.x + buffer.x, -near.y - buffer.y, near.z),
      glm::vec3(              0.0f, -near.y - buffer.y, near.z),
      glm::vec3(-near.x - buffer.x, -near.y - buffer.y, near.z),
      // Outside middle plane.
      glm::vec3( mid.x + buffer.x,  mid.y + buffer.y, mid.z),
      glm::vec3(             0.0f,  mid.y + buffer.y, mid.z),
      glm::vec3(-mid.x - buffer.x,  mid.y + buffer.y, mid.z),
      glm::vec3( mid.x + buffer.x,              0.0f, mid.z),
      glm::vec3(-mid.x - buffer.x,              0.0f, mid.z),
      glm::vec3( mid.x + buffer.x, -mid.y - buffer.y, mid.z),
      glm::vec3(             0.0f, -mid.y - buffer.y, mid.z),
      glm::vec3(-mid.x - buffer.x, -mid.y - buffer.y, mid.z),
      // Outside far plane.
      glm::vec3( far.x + buffer.x,  far.y + buffer.y, far.z),
      glm::vec3(             0.0f,  far.y + buffer.y, far.z),
      glm::vec3(-far.x - buffer.x,  far.y + buffer.y, far.z),
      glm::vec3( far.x + buffer.x,              0.0f, far.z),
      glm::vec3(-far.x - buffer.x,              0.0f, far.z),
      glm::vec3( far.x + buffer.x, -far.y - buffer.y, far.z),
      glm::vec3(             0.0f, -far.y - buffer.y, far.z),
      glm::vec3(-far.x - buffer.x, -far.y - buffer.y, far.z),
      // After far plane.
      glm::vec3( far.x - buffer.x,  far.y - buffer.y, far.z + 1.0f),
      glm::vec3(             0.0f,  far.y - buffer.y, far.z + 1.0f),
      glm::vec3(-far.x + buffer.x,  far.y - buffer.y, far.z + 1.0f),
      glm::vec3( far.x - buffer.x,              0.0f, far.z + 1.0f),
      glm::vec3(-far.x + buffer.x,              0.0f, far.z + 1.0f),
      glm::vec3( far.x - buffer.x, -far.y + buffer.y, far.z + 1.0f),
      glm::vec3(             0.0f, -far.y + buffer.y, far.z + 1.0f),
      glm::vec3(-far.x + buffer.x, -far.y + buffer.y, far.z + 1.0f),
   }};

   glm::mat4 transformMatrix = c.getTransform();
   for (int ndx = 0; ndx < (int)inView.size(); ++ndx) {
      inView[ndx] = glm::vec3(transformMatrix * glm::vec4(inView[ndx], 1.0f));
   }
   for (int ndx = 0; ndx < (int)outOfView.size(); ++ndx) {
      outOfView[ndx] = glm::vec3(transformMatrix * glm::vec4(outOfView[ndx], 1.0f));
   }

   assertPointVisibility(c, inView, outOfView);
}

TEST_CASE("crash/space/camera/simple") {
   assertCamera(crash::math::origin, glm::vec4(crash::math::xAxis, 0.0f),
    /* fov */ glm::radians(60.0f), /* aspect ratio */ 1.0f);
}

TEST_CASE("crash/space/camera/wide") {
   assertCamera(crash::math::origin, glm::vec4(crash::math::xAxis, 0.0f),
    /* fov */ glm::radians(60.0f), /* aspect ratio */ 2.0f);
}

TEST_CASE("crash/space/camera/skewed") {
   assertCamera(glm::vec3(1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 30.0f),
    /* fov */ glm::radians(80.0f), /* aspect ratio */ 16.0f / 9.0f);
}
