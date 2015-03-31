#include <catch.hpp>
#include <cmath>
#include <array>
#include <vector>
#include <crash/math/arithmetic.hpp>
#include <crash/math/symbols.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/camera.hpp>

using namespace crash::math;
using namespace crash::space;

static void assertPointVisibility(Camera& camera,
 const std::vector< glm::vec3 >& inView,
 const std::vector< glm::vec3 >& outOfView) {
   auto& vf = camera.getViewFrustum();
   for (auto point : inView) {
      REQUIRE(vf.isPointVisible(point));
   }
   for (auto point : outOfView) {
      REQUIRE(!vf.isPointVisible(point));
   }
}

static void assertCamera(Camera c) {
   auto fov = c.getFieldOfView();
   auto aspect = c.getAspectRatio();
   float tanX = tan(fov * aspect * 0.5f);
   float tanY = tan(fov * 0.5f);
   glm::vec3 near = glm::vec3(tanX, tanY, 1.0f);
   glm::vec3 far = 10.0f * near;
   glm::vec3 mid = average(near, far);
   glm::vec3 buffer = glm::vec3(tanX * 1.5f, tanY * 1.5f, 1.0f);

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

   assertPointVisibility(c, inView, outOfView);
}

TEST_CASE("crash/space/camera/simple") {
   assertCamera(Camera(origin, -zAxis, yAxis,
    /* fov */ glm::radians(60.0f), /* aspect ratio */ 1.0f,
    /* near */ 1.0f, /* far */ 10.0f));
}

TEST_CASE("crash/space/camera/wide") {
   assertCamera(Camera(origin, -zAxis, yAxis,
    /* fov */ glm::radians(60.0f), /* aspect ratio */ 2.0f,
    /* near */ 1.0f, /* far */ 10.0f));
}
