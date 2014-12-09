#include <array>
#include <cmath>
#include <vector>
#include <crash/math/math.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/camera.hpp>
#include <catch.hpp>

using namespace crash::space;

static std::array< glm::vec3, 9 > buildPointSet(const glm::vec3& nearPos,
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

static void assertVisibility(Camera& camera,
 const std::vector< Boundable >& inView,
 const std::vector< Boundable >& outOfView) {
   ViewFrustum vf = camera.getViewFrustum();
   for (auto boundable : inView) {
      REQUIRE(boundable.isVisible(vf));
   }
   for (auto boundable : outOfView) {
      REQUIRE(!boundable.isVisible(vf));
   }
}

static void assertIntersection(Boundable& reference,
 const std::vector< Boundable >& intersecting,
 const std::vector< Boundable >& notIntersecting) {
   for (auto boundable : intersecting) {
      REQUIRE(boundable.intersect(reference));
   }
   for (auto boundable : notIntersecting) {
      REQUIRE(!boundable.intersect(reference));
   }
}

TEST_CASE("crash/space/boundable/is_visible") {
   float fov = glm::radians(45.0f);
   float aspect = 1.0f;
   float tanX = tan(fov * aspect * 0.5f);
   float tanY = tan(fov * 0.5f);
   glm::vec3 near = glm::vec3(tanX, tanY, 1.0f);
   glm::vec3 far = 10.0f * near;

   Camera c = Camera(crash::math::origin, glm::vec4(crash::math::zAxis, 0.0f),
    fov, aspect, near.z, far.z);

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

   std::array< std::array< glm::vec3, 9 >, 6 > pointSets = {{
      buildPointSet(corners[2], corners[0], corners[6], corners[4]), // left
      buildPointSet(corners[3], corners[1], corners[7], corners[5]), // right
      buildPointSet(corners[2], corners[3], corners[6], corners[7]), // top
      buildPointSet(corners[0], corners[1], corners[4], corners[5]), // bottom
      buildPointSet(corners[2], corners[0], corners[3], corners[1]), // near
      buildPointSet(corners[6], corners[4], corners[7], corners[5])  // far
   }};

   std::array< glm::vec3, 6 > pointSetOffsets = {{
      glm::vec3(-2.0f,  0.0f,  0.0f), // left
      glm::vec3( 2.0f,  0.0f,  0.0f), // right
      glm::vec3( 0.0f,  2.0f,  0.0f), // top
      glm::vec3( 0.0f, -2.0f,  0.0f), // bottom
      glm::vec3( 0.0f,  0.0f, -2.0f), // near
      glm::vec3( 0.0f,  0.0f,  2.0f)  // far
   }};

   std::vector< Boundable > inView;
   std::vector< Boundable > outOfView;
   glm::mat4 transformMatrix = c.getTransform();
   for (int setNdx = 0; setNdx < 6; ++setNdx) {
      std::array< glm::vec3, 9 >& pointSet = pointSets[setNdx];
      for (int pointNdx = 0; pointNdx < (int)pointSet.size(); ++pointNdx) {
         glm::vec3 position = pointSet[pointNdx];
         glm::vec3 transformedPosition = glm::vec3(transformMatrix *
          glm::vec4(pointSet[pointNdx] + pointSetOffsets[setNdx], 1.0f));

         inView.push_back(Boundable(position));
         outOfView.push_back(Boundable(transformedPosition));
      }
   }

   assertVisibility(c, inView, outOfView);
}

TEST_CASE("crash/space/boundable/intersect") {
   Boundable reference = Boundable();

   glm::vec3 xPosition = glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 yPosition = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 zPosition = glm::vec3(0.0f, 0.0f, 1.0f);

   glm::vec4 nOrientation = glm::vec4(crash::math::xAxis,  0.0f);
   glm::vec4 xOrientation = glm::vec4(crash::math::xAxis, 45.0f);
   glm::vec4 yOrientation = glm::vec4(crash::math::yAxis, 45.0f);
   glm::vec4 zOrientation = glm::vec4(crash::math::zAxis, 45.0f);

   glm::vec3 smallSize = glm::vec3(0.5f);
   glm::vec3 mediumSize = glm::vec3(1.0f);
   glm::vec3 bigSize = glm::vec3(2.0f);

   std::vector< Boundable > intersecting = {{
      Boundable(xPosition, nOrientation, mediumSize),
      Boundable(yPosition, nOrientation, mediumSize),
      Boundable(zPosition, nOrientation, mediumSize),
      Boundable(xPosition, xOrientation, mediumSize),
      Boundable(yPosition, xOrientation, mediumSize),
      Boundable(zPosition, xOrientation, mediumSize),
      Boundable(xPosition, yOrientation, mediumSize),
      Boundable(yPosition, yOrientation, mediumSize),
      Boundable(zPosition, yOrientation, mediumSize),
      Boundable(xPosition, zOrientation, mediumSize),
      Boundable(yPosition, zOrientation, mediumSize),
      Boundable(zPosition, zOrientation, mediumSize),
      Boundable(xPosition, nOrientation, bigSize),
      Boundable(yPosition, nOrientation, bigSize),
      Boundable(zPosition, nOrientation, bigSize),
      Boundable(xPosition, xOrientation, bigSize),
      Boundable(yPosition, xOrientation, bigSize),
      Boundable(zPosition, xOrientation, bigSize),
      Boundable(xPosition, yOrientation, bigSize),
      Boundable(yPosition, yOrientation, bigSize),
      Boundable(zPosition, yOrientation, bigSize),
      Boundable(xPosition, zOrientation, bigSize),
      Boundable(yPosition, zOrientation, bigSize),
      Boundable(zPosition, zOrientation, bigSize),
      Boundable(xPosition * -1.0f, nOrientation, mediumSize),
      Boundable(yPosition * -1.0f, nOrientation, mediumSize),
      Boundable(zPosition * -1.0f, nOrientation, mediumSize),
      Boundable(xPosition * -1.0f, xOrientation, mediumSize),
      Boundable(yPosition * -1.0f, xOrientation, mediumSize),
      Boundable(zPosition * -1.0f, xOrientation, mediumSize),
      Boundable(xPosition * -1.0f, yOrientation, mediumSize),
      Boundable(yPosition * -1.0f, yOrientation, mediumSize),
      Boundable(zPosition * -1.0f, yOrientation, mediumSize),
      Boundable(xPosition * -1.0f, zOrientation, mediumSize),
      Boundable(yPosition * -1.0f, zOrientation, mediumSize),
      Boundable(zPosition * -1.0f, zOrientation, mediumSize),
      Boundable(xPosition * -1.0f, nOrientation, bigSize),
      Boundable(yPosition * -1.0f, nOrientation, bigSize),
      Boundable(zPosition * -1.0f, nOrientation, bigSize),
      Boundable(xPosition * -1.0f, xOrientation, bigSize),
      Boundable(yPosition * -1.0f, xOrientation, bigSize),
      Boundable(zPosition * -1.0f, xOrientation, bigSize),
      Boundable(xPosition * -1.0f, yOrientation, bigSize),
      Boundable(yPosition * -1.0f, yOrientation, bigSize),
      Boundable(zPosition * -1.0f, yOrientation, bigSize),
      Boundable(xPosition * -1.0f, zOrientation, bigSize),
      Boundable(yPosition * -1.0f, zOrientation, bigSize),
      Boundable(zPosition * -1.0f, zOrientation, bigSize),
   }};

   std::vector< Boundable > notIntersecting = {{
      Boundable(xPosition, nOrientation, smallSize),
      Boundable(yPosition, nOrientation, smallSize),
      Boundable(zPosition, nOrientation, smallSize),
      Boundable(xPosition, xOrientation, smallSize),
      Boundable(yPosition, xOrientation, smallSize),
      Boundable(zPosition, xOrientation, smallSize),
      Boundable(xPosition, yOrientation, smallSize),
      Boundable(yPosition, yOrientation, smallSize),
      Boundable(zPosition, yOrientation, smallSize),
      Boundable(xPosition, zOrientation, smallSize),
      Boundable(yPosition, zOrientation, smallSize),
      Boundable(zPosition, zOrientation, smallSize),
      Boundable(xPosition * -1.0f, nOrientation, smallSize),
      Boundable(yPosition * -1.0f, nOrientation, smallSize),
      Boundable(zPosition * -1.0f, nOrientation, smallSize),
      Boundable(xPosition * -1.0f, xOrientation, smallSize),
      Boundable(yPosition * -1.0f, xOrientation, smallSize),
      Boundable(zPosition * -1.0f, xOrientation, smallSize),
      Boundable(xPosition * -1.0f, yOrientation, smallSize),
      Boundable(yPosition * -1.0f, yOrientation, smallSize),
      Boundable(zPosition * -1.0f, yOrientation, smallSize),
      Boundable(xPosition * -1.0f, zOrientation, smallSize),
      Boundable(yPosition * -1.0f, zOrientation, smallSize),
      Boundable(zPosition * -1.0f, zOrientation, smallSize),
   }};

   assertIntersection(reference, intersecting, notIntersecting);
}
