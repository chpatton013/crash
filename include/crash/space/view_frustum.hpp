#pragma once

#include <array>
#include <glm/glm.hpp>
#include <crash/math/plane.hpp>

namespace crash {
namespace space {

class ViewFrustum {
public:
   static const int NUM_PLANES = 6;
   static const int NUM_CORNERS = 8;

   typedef std::array< math::Plane, NUM_PLANES > Planes;
   typedef std::array< glm::vec3, NUM_CORNERS > Corners3;
   typedef std::array< glm::vec4, NUM_CORNERS > Corners4;

   ViewFrustum(const ViewFrustum& viewFrustum);
   ViewFrustum(const Planes& planes);

   const Planes& getPlanes() const;

   static ViewFrustum fromValues(float fieldOfView, float aspectRatio,
    float nearPlane, float farPlane, const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(const Corners3& corners,
    const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(const Corners3& corners);

   static ViewFrustum fromCorners(const Corners4& corners,
    const glm::mat4& transformMatrix);

   static ViewFrustum fromCorners(const Corners4& corners);

private:
   Planes _planes;
};

} // namespace space
} // namespace crash
