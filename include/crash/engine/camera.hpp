#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/renderable.hpp>
#include <crash/render/view_frustum.hpp>

namespace crash {
namespace engine {

class Camera;
typedef std::shared_ptr< Camera > CameraPtr;

class Camera : public space::Boundable {
public:
   Camera(const Camera& camera);
   Camera(const space::BoundingBox& boundingBox,
    float verticalFieldOfView, float aspectRatio,
    float nearPlane, float farPlane);
   virtual ~Camera();

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

   void translate(const glm::vec3& translation);
   void rotate(const glm::quat& rotation);
   void scale(const glm::vec3& scale);

   void translate(float delta_t);
   void rotate(float delta_t);
   void scale(float delta_t);

   void move(float delta_t);

   /////////////////////////////////////////////////////////////////////////////
   // Boundable interface.
   /////////////////////////////////////////////////////////////////////////////

   space::BoundingBox* getBoundingBox();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   float getVerticalFieldOfView() const;
   float getAspectRatio() const;
   float getNearPlane() const;
   float getFarPlane() const;

   void setVerticalFieldOfView(float verticalFieldOfView);
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

   const render::ViewFrustum& getViewFrustum();

private:
   void calculateOrthographic();
   void calculatePerspective();
   void calculateLookAt();
   void calculateViewFrustum();

   space::BoundingBox _boundingBox;
   float _verticalFieldOfView;
   float _aspectRatio;
   float _nearPlane;
   float _farPlane;

   boost::optional< glm::mat4 > _orthographic;
   boost::optional< glm::mat4 > _perspective;
   boost::optional< glm::mat4 > _lookAt;
   boost::optional< render::ViewFrustum > _viewFrustum;
};

class RenderableCamera : public Camera, public render::Renderable {
public:
   RenderableCamera(const RenderableCamera& camera);
   RenderableCamera(const Camera& camera,
    const render::MeshInstance& _meshInstance);
   virtual ~RenderableCamera();

   /////////////////////////////////////////////////////////////////////////////
   // Renderable interface.
   /////////////////////////////////////////////////////////////////////////////

   render::MeshInstance* getMeshInstance();
   void render(float delta_t);

private:
   render::MeshInstance _meshInstance;
};

} // namespace engine
} // namespace crash
