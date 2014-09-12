#pragma once

#include <array>
#include <glm/glm.hpp>
#include <crash/math/plane.hpp>

namespace crash {
namespace space {

const int NUM_VIEW_FRUSTUM_PLANES = 6;
const int NUM_VIEW_FRUSTUM_CORNERS = 8;

class ViewFrustum {
public:
   ViewFrustum(const std::array< math::Plane, NUM_VIEW_FRUSTUM_PLANES > planes);

   const std::array< math::Plane, NUM_VIEW_FRUSTUM_PLANES > getPlanes() const;

   static ViewFrustum fromValues(float fieldOfView, float aspectRatio,
    float nearPlane, float farPlane, const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(
    const std::array< glm::vec3, NUM_VIEW_FRUSTUM_CORNERS >& corners,
    const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(
    const std::array< glm::vec3, NUM_VIEW_FRUSTUM_CORNERS >& corners);

   static ViewFrustum fromCorners(
    const std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS >& corners,
    const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(
    const std::array< glm::vec4, NUM_VIEW_FRUSTUM_CORNERS >& corners);

private:
   std::array< math::Plane, NUM_VIEW_FRUSTUM_PLANES > _planes;
};

} // namespace space
} // namespace crash
