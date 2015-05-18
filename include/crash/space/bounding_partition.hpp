#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/common/movable.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/bounding_group.hpp>

namespace crash {

namespace render {
   class ViewFrustum;
}

namespace space {

class Collision;

class BoundingPartition : public common::Movable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingPartition(const BoundingPartition& spatialManager);
   BoundingPartition(const common::Transformer& transformer,
    const glm::ivec3& partitions);

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

   void resize(const common::Transformer& transformer,
    const glm::ivec3& partitions);
   void partition(const common::Transformer& transformer,
    const glm::ivec3& partitions);

   /////////////////////////////////////////////////////////////////////////////
   // Grouping.
   /////////////////////////////////////////////////////////////////////////////

   bool add(BoundingBox* boundable);
   bool remove(BoundingBox* boundable);
   bool update(BoundingBox* boundable);
   void clear();

   unsigned int getNumBoundingBoxes() const;
   unsigned int getNumBoundingGroups() const;

   std::vector< BoundingBox* > getBoundingBoxes() const;
   std::vector< BoundingGroup > getBoundingGroups() const;

   const glm::ivec3& getPartitions() const;

   /////////////////////////////////////////////////////////////////////////////
   // Spatial queries.
   /////////////////////////////////////////////////////////////////////////////

   bool isVisible(const render::ViewFrustum& viewFrustum);
   bool isIntersect(BoundingBox& other);

   std::vector< BoundingGroup > getContainingBoundingGroups(
    BoundingBox* boundingBox) const;
   std::vector< Collision > getCollidingElements() const;
   std::vector< BoundingBox* > getVisibleElements(
    const render::ViewFrustum& viewFrustum) const;

private:
   BoundingBox _boundingBox;
   glm::ivec3 _partitions;
   std::vector< BoundingGroup > _boundingGroups;
   unsigned int _numBoundingBoxes;
};

} // namespace space
} // namespace crash
