#pragma once

#include <set>
#include <vector>
#include <crash/space/bounding_box.hpp>
#include <crash/space/collision.hpp>

namespace crash {
namespace space {

class ViewFrustum;

class BoundingGroup : public Movable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingGroup(const BoundingGroup& boundingGroup);
   BoundingGroup(const Transformer& transformer);

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

   const BoundingBox& getBoundingBox() const;

   /////////////////////////////////////////////////////////////////////////////
   // Grouping.
   /////////////////////////////////////////////////////////////////////////////

   bool add(BoundingBox* boundingBox);
   bool remove(BoundingBox* boundingBox);
   void clear();
   const std::set< BoundingBox* >& getBoundingBoxes() const;

   /////////////////////////////////////////////////////////////////////////////
   // Spatial queries.
   /////////////////////////////////////////////////////////////////////////////

   bool isVisible(const ViewFrustum& viewFrustum);
   bool isIntersecting(BoundingBox& other);

   std::vector< Collision > getCollidingElements() const;
   std::vector< BoundingBox* > getVisibleElements(
    const ViewFrustum& viewFrustum) const;

protected:
   BoundingBox _boundingBox;
   std::set< BoundingBox* > _boundingBoxes;
};

} // namespace space
} // namespace crash
