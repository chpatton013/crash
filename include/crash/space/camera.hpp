#pragma once

#include <glm/glm.hpp>
#include <boost/optional.hpp>
#include <crash/space/view_frustum.hpp>

namespace crash {
namespace space {

class Camera {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Camera(const Camera& camera);
   Camera(const glm::vec3& position, const glm::vec3& forward,
    const glm::vec3& up, float fieldOfView, float aspectRatio,
    float nearPlane, float farPlane);

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   const glm::vec3& getPosition() const;
   const glm::vec3& getForward() const;
   const glm::vec3& getUp() const;
   float getFieldOfView() const;
   float getAspectRatio() const;
   float getNearPlane() const;
   float getFarPlane() const;

   void setPosition(const glm::vec3& position);
   void setForward(const glm::vec3& forward);
   void setUp(const glm::vec3& up);
   void setFieldOfView(float fieldOfView);
   void setAspectRatio(float aspectRatio);
   void setNearPlane(float nearPlane);
   void setFarPlane(float farPlane);

   /////////////////////////////////////////////////////////////////////////////
   // Memoization
   /////////////////////////////////////////////////////////////////////////////

   void invalidate();

   const glm::mat4& getOrthographic();
   const glm::mat4& getPerspective();
   const glm::mat4& getLookAt();

   const ViewFrustum& getViewFrustum();

private:
   // Data members
   glm::vec3 _position;
   glm::vec3 _forward;
   glm::vec3 _up;
   float _fieldOfView;
   float _aspectRatio;
   float _nearPlane;
   float _farPlane;

   // Memoization
   void calculateOrthographic();
   void calculatePerspective();
   void calculateLookAt();
   void calculateViewFrustum();

   boost::optional< glm::mat4 > _orthographic;
   boost::optional< glm::mat4 > _perspective;
   boost::optional< glm::mat4 > _lookAt;
   boost::optional< ViewFrustum > _viewFrustum;
};

} // namespace space
} // namespace crash
