#pragma once

#include <glm/glm.hpp>
#include <crash/math/transformable.hpp>
#include <crash/math/transformer.hpp>
#include <crash/space/view_frustum.hpp>

namespace crash {
namespace space {

class Camera : public math::Transformable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   Camera(const Camera& camera);
   Camera(const math::Transformer& transformer, float fieldOfView,
    float aspectRatio, float nearPlane, float farPlane);

   /////////////////////////////////////////////////////////////////////////////
   // Transformable interface.
   /////////////////////////////////////////////////////////////////////////////

   const glm::vec3& getPosition() const;
   const glm::vec4& getOrientation() const;
   const glm::vec3& getSize() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::vec4& orientation);
   void setSize(const glm::vec3& size);

   const glm::mat4& getTransform();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   float getFieldOfView() const;
   float getAspectRatio() const;
   float getNearPlane() const;
   float getFarPlane() const;
   const math::Transformer& getTransformer() const;

   void setFieldOfView(float fieldOfView);
   void setAspectRatio(float aspectRatio);
   void setNearPlane(float nearPlane);
   void setFarPlane(float farPlane);
   void setTransformer(const math::Transformer& transformer);

   /////////////////////////////////////////////////////////////////////////////
   // Memoization
   /////////////////////////////////////////////////////////////////////////////

   void invalidate();

   /**
    * Calculate the perspective matrix for this Camera.
    */
   const glm::mat4& getPerspective();

   /**
    * Calculate the ViewingFrustum for this Camera.
    */
   const ViewFrustum& getViewFrustum();

private:
   // Data members
   math::Transformer _transformer;
   float _fieldOfView;
   float _aspectRatio;
   float _nearPlane;
   float _farPlane;

   // Memoization
   void calculatePerspective();
   void calculateViewFrustum();

   boost::optional< glm::mat4 > _perspective;
   boost::optional< ViewFrustum > _viewFrustum;
};

} // namespace space
} // namespace crash
