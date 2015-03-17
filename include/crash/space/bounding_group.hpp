#pragma once

#include <set>
#include <vector>
#include <crash/math/transformable.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/collision.hpp>

namespace crash {

namespace math {
   class Transformer;
}

namespace space {

class ViewFrustum;

class BoundingGroup : public math::Transformable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingGroup(const BoundingGroup& boundingGroup);
   BoundingGroup(const math::Transformer& transformer);

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
