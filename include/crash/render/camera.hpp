#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <boost/optional.hpp>
#include <crash/common/movable.hpp>
#include <crash/render/view_frustum.hpp>

namespace crash {
namespace render {

class Camera : public common::Movable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Camera(const Camera& camera);
   Camera(const glm::vec3& position, const glm::quat& orientation,
    float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

   /////////////////////////////////////////////////////////////////////////////
   // Movable interface.
   /////////////////////////////////////////////////////////////////////////////

   glm::vec3 getPosition() const;
   glm::quat getOrientation() const;
   glm::vec3 getSize() const;
   glm::vec3 getTranslationalVelocity() const;
   glm::quat getRotationalVelocity() const;
   glm::vec3 getScaleVelocity() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::quat& orientation);
   void setSize(const glm::vec3& size);
   void setTranslationalVelocity(const glm::vec3& translationalVelocity);
   void setRotationalVelocity(const glm::quat& rotationalVelocity);
   void setScaleVelocity(const glm::vec3& scaleVelocity);

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   float getFieldOfView() const;
   float getAspectRatio() const;
   float getNearPlane() const;
   float getFarPlane() const;

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
   glm::quat _orientation;
   glm::vec3 _translationalVelocity;
   glm::quat _rotationalVelocity;
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

} // namespace render
} // namespace crash
