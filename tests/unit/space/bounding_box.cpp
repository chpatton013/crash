#include <catch.hpp>
#include <cmath>
#include <array>
#include <vector>
#include <crash/math/arithmetic.hpp>
#include <crash/math/transformer.hpp>
#include <crash/math/symbols.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/camera.hpp>

using namespace crash::math;
using namespace crash::space;

static std::array< glm::vec3, 9 > buildPointSet(const glm::vec3& nearPos,
 const glm::vec3& nearNeg, const glm::vec3& farPos, const glm::vec3& farNeg) {
   glm::vec3 nearAvg = average(nearPos, nearNeg);
   glm::vec3 farAvg = average(farPos, farNeg);
   glm::vec3 midPos = average(farPos, nearPos);
   glm::vec3 midNeg = average(farNeg, nearNeg);
   glm::vec3 midAvg = average(midPos, midNeg);

   return {{
      nearPos, nearAvg, nearNeg,
      midPos, midAvg, midNeg,
      farPos, farAvg, farNeg
   }};
}

static void assertVisibility(ViewFrustum& vf,
 const std::vector< BoundingBox >& inView,
 const std::vector< BoundingBox >& outOfView) {
   for (auto boundingBox : inView) {
      REQUIRE(boundingBox.isVisible(vf));
   }
   for (auto boundingBox : outOfView) {
      REQUIRE(!boundingBox.isVisible(vf));
   }
}

static void assertIntersection(BoundingBox& reference,
 const std::vector< BoundingBox >& intersecting,
 const std::vector< BoundingBox >& notIntersecting) {
   for (auto boundingBox : intersecting) {
      REQUIRE(boundingBox.isIntersecting(reference));
   }
   for (auto boundingBox : notIntersecting) {
      REQUIRE(!boundingBox.isIntersecting(reference));
   }
}

TEST_CASE("crash/space/bounding_box/is_visible") {
   float fov = glm::radians(45.0f);
   float aspect = 1.0f;
   float tanX = tan(fov * aspect * 0.5f);
   float tanY = tan(fov * 0.5f);
   glm::vec3 near = glm::vec3(tanX, tanY, 1.0f);
   glm::vec3 far = 10.0f * near;

   Camera c = Camera(Transformer(origin, glm::vec4(zAxis, 0.0f), glm::vec3(1.0f)),
    fov, aspect, near.z, far.z);
   ViewFrustum vf = c.getViewFrustum();

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

   std::vector< BoundingBox > inView;
   std::vector< BoundingBox > outOfView;
   glm::mat4 transformMatrix = c.getTransform();
   for (int setNdx = 0; setNdx < 6; ++setNdx) {
      std::array< glm::vec3, 9 >& pointSet = pointSets[setNdx];
      for (int pointNdx = 0; pointNdx < (int)pointSet.size(); ++pointNdx) {
         glm::vec3 position = pointSet[pointNdx];
         glm::vec3 transformedPosition = glm::vec3(transformMatrix *
          glm::vec4(pointSet[pointNdx] + pointSetOffsets[setNdx], 1.0f));

         inView.push_back(BoundingBox(
          Transformer(position, glm::vec4(zAxis, 0.0f), glm::vec3(1.0f)),
          glm::vec3(), glm::vec4(xAxis, 0.0f)));
         outOfView.push_back(BoundingBox(
          Transformer(transformedPosition, glm::vec4(zAxis, 0.0f), glm::vec3(1.0f)),
          glm::vec3(), glm::vec4(xAxis, 0.0f)));
      }
   }

   assertVisibility(vf, inView, outOfView);
}

TEST_CASE("crash/space/bounding_box/intersect") {
   BoundingBox reference = BoundingBox(
    Transformer(glm::vec3(), glm::vec4(xAxis, 0.0f), glm::vec3()),
    glm::vec3(), glm::vec4(xAxis, 0.0f));

   glm::vec3 xPosition = glm::vec3(1.0f, 0.0f, 0.0f);
   glm::vec3 yPosition = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 zPosition = glm::vec3(0.0f, 0.0f, 1.0f);

   glm::vec4 nOrientation = glm::vec4(xAxis,  0.0f);
   glm::vec4 xOrientation = glm::vec4(xAxis, 45.0f);
   glm::vec4 yOrientation = glm::vec4(yAxis, 45.0f);
   glm::vec4 zOrientation = glm::vec4(zAxis, 45.0f);

   glm::vec3 smallSize = glm::vec3(0.5f);
   glm::vec3 mediumSize = glm::vec3(1.0f);
   glm::vec3 bigSize = glm::vec3(2.0f);

   std::vector< BoundingBox > intersecting = {{
      BoundingBox(Transformer(xPosition, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, nOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, xOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, yOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, zOrientation, mediumSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, nOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, xOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, yOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, zOrientation, bigSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
   }};

   std::vector< BoundingBox > notIntersecting = {{
      BoundingBox(Transformer(xPosition, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, nOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, xOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, yOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(xPosition * -1.0f, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(yPosition * -1.0f, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      BoundingBox(Transformer(zPosition * -1.0f, zOrientation, smallSize), glm::vec3(), glm::vec4(xAxis, 0.0f)),
   }};

   assertIntersection(reference, intersecting, notIntersecting);
}
