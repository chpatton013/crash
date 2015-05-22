#pragma once

#include <set>
#include <vector>
#include <crash/common/movable.hpp>
#include <crash/common/transformer.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/collision.hpp>

namespace crash {

namespace render {
   class ViewFrustum;
}

namespace space {

class BoundingGroup : public common::Movable, public Boundable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingGroup(const BoundingGroup& boundingGroup);
   BoundingGroup(const common::Transformer& transformer);

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

   BoundingBox* getBoundingBox();

   /////////////////////////////////////////////////////////////////////////////
   // Grouping.
   /////////////////////////////////////////////////////////////////////////////

   bool add(Boundable* boundingBox);
   bool remove(Boundable* boundingBox);
   void clear();
   const std::set< Boundable* >& getBoundables() const;

   std::vector< Collision > getCollidingElements() const;
   std::vector< Boundable* > getVisibleElements(
    const render::ViewFrustum& viewFrustum) const;

protected:
   BoundingBox _boundingBox;
   std::set< Boundable* > _boundables;
};

} // namespace space
} // namespace crash
