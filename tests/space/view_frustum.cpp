#include <array>
#include <cmath>
#include <vector>
#include <crash/math/math.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/view_frustum.hpp>
#include <catch.hpp>

using namespace crash::space;

static std::vector< glm::vec3 > buildPointSet(const glm::vec3& nearPos,
 const glm::vec3& nearNeg, const glm::vec3& farPos, const glm::vec3& farNeg) {
   glm::vec3 nearAvg = crash::math::average(nearPos, nearNeg);
   glm::vec3 farAvg = crash::math::average(farPos, farNeg);
   glm::vec3 midPos = crash::math::average(farPos, nearPos);
   glm::vec3 midNeg = crash::math::average(farNeg, nearNeg);
   glm::vec3 midAvg = crash::math::average(midPos, midNeg);

   return {{
      nearPos, nearAvg, nearNeg,
      midPos, midAvg, midNeg,
      farPos, farAvg, farNeg
   }};
}

static void assertPlaneLocation(const crash::math::Plane& plane,
 const std::vector< glm::vec3 >& points) {
   for (auto point : points) {
      REQUIRE(crash::math::approxEqual(plane.distance(point), 0.0f));
      REQUIRE(crash::math::approxGreaterThan(plane.distance(point + plane.normal), 0.0f));
      REQUIRE(crash::math::approxLessThan(plane.distance(point - plane.normal), 0.0f));
   }
}

TEST_CASE("crash/space/view_frustum") {
   glm::vec3 position = crash::math::origin;
   glm::vec4 orientation = glm::vec4(crash::math::xAxis, 0.0f);
   float fov = glm::radians(60.0f);
   float aspect = 1.0f;
   float tanX = tan(fov * aspect * 0.5f);
   float tanY = tan(fov * 0.5f);
   glm::vec3 near = glm::vec3(tanX, tanY, 1.0f);
   glm::vec3 far = 10.0f * near;
   glm::vec3 mid = crash::math::average(near, far);

   std::array< glm::vec3, 8 > corners = {{
      glm::vec3(-near.x, -near.y, near.z), // 0:nbl
      glm::vec3( near.x, -near.y, near.z), // 1:nbr
      glm::vec3(-near.x,  near.y, near.z), // 2:ntl
      glm::vec3( near.x,  near.y, near.z), // 3:ntr
      glm::vec3( -far.x,  -far.y,  far.z), // 4:fbl
      glm::vec3(  far.x,  -far.y,  far.z), // 5:fbr
      glm::vec3( -far.x,   far.y,  far.z), // 6:ftl
      glm::vec3(  far.x,   far.y,  far.z), // 7:ftr
   }};

   std::vector< std::vector< glm::vec3 > > pointSets = {{
      buildPointSet(corners[2], corners[0], corners[6], corners[4]), // left
      buildPointSet(corners[3], corners[1], corners[7], corners[5]), // right
      buildPointSet(corners[2], corners[3], corners[6], corners[7]), // top
      buildPointSet(corners[0], corners[1], corners[4], corners[5]), // bottom
      buildPointSet(corners[2], corners[0], corners[3], corners[1]), // near
      buildPointSet(corners[6], corners[4], corners[7], corners[5])  // far
   }};

   ViewFrustum vf = ViewFrustum::fromValues(fov, aspect, near.z, far.z, glm::mat4());

   auto planes = vf.getPlanes();
   for (int ndx = 0; ndx < NUM_VIEW_FRUSTUM_PLANES; ++ndx) {
      assertPlaneLocation(planes[ndx], pointSets[ndx]);
   }
}
