#pragma once

#include <memory>
#include <crash/common/movable.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/renderable.hpp>

namespace crash {
namespace engine {

class Actor;
typedef std::shared_ptr< Actor > ActorPtr;

class Actor : public space::Boundable, public render::Renderable {
public:
   Actor(const Actor& actor);
   Actor(const space::BoundingBox& _boundingBox,
    const render::MeshInstance& _meshInstance);
   virtual ~Actor();

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
   // Boundable interface.
   /////////////////////////////////////////////////////////////////////////////

   space::BoundingBox* getBoundingBox();

   /////////////////////////////////////////////////////////////////////////////
   // Renderable interface.
   /////////////////////////////////////////////////////////////////////////////

   render::MeshInstance* getMeshInstance();
   void render(float delta_t);

private:
   space::BoundingBox _boundingBox;
   render::MeshInstance _meshInstance;
};

} // namespace engine
} // namespace crash
