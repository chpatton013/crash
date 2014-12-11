#include <crash/space/view_frustum.hpp>

using namespace crash::space;

ViewFrustum::ViewFrustum(
 const std::array< math::Plane, NUM_VIEW_FRUSTUM_PLANES > planes) :
   _planes(planes)
{}

const std::array< crash::math::Plane, NUM_VIEW_FRUSTUM_PLANES >
 ViewFrustum::getPlanes() const {
   return this->_planes;
}

/* static */ ViewFrustum ViewFrustum::fromValues(float fieldOfView, float aspectRatio,
 float nearPlane, float farPlane, const glm::mat4& transformMatrix) {
   float tanX = tan(fieldOfView * aspectRatio * 0.5f);
   float tanY = tan(fieldOfView * 0.5f);
   float nearX = tanX * nearPlane;
   float nearY = tanY * nearPlane;
   float farX = tanX * farPlane;
   float farY = tanY * farPlane;

   std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS > corners = {{
      glm::vec4(-nearX, -nearY, nearPlane, 1.0f), // 0:nbl
      glm::vec4( nearX, -nearY, nearPlane, 1.0f), // 1:nbr
      glm::vec4(-nearX,  nearY, nearPlane, 1.0f), // 2:ntl
      glm::vec4( nearX,  nearY, nearPlane, 1.0f), // 3:ntr
      glm::vec4( -farX,  -farY,  farPlane, 1.0f), // 4:fbl
      glm::vec4(  farX,  -farY,  farPlane, 1.0f), // 5:fbr
      glm::vec4( -farX,   farY,  farPlane, 1.0f), // 6:ftl
      glm::vec4(  farX,   farY,  farPlane, 1.0f), // 7:ftr
   }};

   return ViewFrustum::fromCorners(corners, transformMatrix);
}

/* static */ ViewFrustum ViewFrustum::fromCorners(
 const std::array< glm::vec3, NUM_VIEW_FRUSTUM_CORNERS >& corners,
 const glm::mat4& transformMatrix) {
   std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS > wideCorners;
   for (int ndx = 0; ndx < NUM_VIEW_FRUSTUM_CORNERS; ++ndx) {
      wideCorners[ndx] = glm::vec4(corners[ndx], 1.0f);
   }

   return ViewFrustum::fromCorners(wideCorners, transformMatrix);
}

/* static */ ViewFrustum ViewFrustum::fromCorners(
 const std::array< glm::vec3, NUM_VIEW_FRUSTUM_CORNERS >& corners) {
   std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS > wideCorners;
   for (int ndx = 0; ndx < NUM_VIEW_FRUSTUM_CORNERS; ++ndx) {
      wideCorners[ndx] = glm::vec4(corners[ndx], 1.0f);
   }

   return ViewFrustum::fromCorners(wideCorners);
}

/* static */ ViewFrustum ViewFrustum::fromCorners(
 const std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS >& corners,
 const glm::mat4& transformMatrix) {
   std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS > transformedCorners;
   for (int ndx = 0; ndx < NUM_VIEW_FRUSTUM_CORNERS; ++ndx) {
      transformedCorners[ndx] = transformMatrix * corners[ndx];
   }

   return ViewFrustum::fromCorners(transformedCorners);
}

/* static */ ViewFrustum ViewFrustum::fromCorners(
 const std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS >& corners) {
   std::array< math::Plane, NUM_VIEW_FRUSTUM_PLANES > planes {{
      math::Plane::fromPoints(corners[0], corners[4], corners[6]), // 0:left
      math::Plane::fromPoints(corners[3], corners[7], corners[5]), // 1:right
      math::Plane::fromPoints(corners[2], corners[6], corners[7]), // 2:top
      math::Plane::fromPoints(corners[1], corners[5], corners[4]), // 3:bottom
      math::Plane::fromPoints(corners[1], corners[0], corners[2]), // 4:near
      math::Plane::fromPoints(corners[4], corners[5], corners[7]), // 5:far
   }};
   return ViewFrustum(planes);
}
