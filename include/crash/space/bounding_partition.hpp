#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/bounding_group.hpp>

namespace crash {

namespace math {
   class Transformer;
}

namespace space {

class Collision;
class ViewFrustum;

class BoundingPartition {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors
   /////////////////////////////////////////////////////////////////////////////

   BoundingPartition(const BoundingPartition& spatialManager);
   BoundingPartition(const math::Transformer& transformer,
    const glm::ivec3& partitions);

   /////////////////////////////////////////////////////////////////////////////
   // Data access
   /////////////////////////////////////////////////////////////////////////////

   const BoundingBox& getBoundingBox() const;
   const glm::vec3& getPosition() const;
   const glm::vec4& getOrientation() const;
   const glm::vec3& getSize() const;

   void resize(const math::Transformer& transformer,
    const glm::ivec3& partitions);
   void partition(const math::Transformer& transformer,
    const glm::ivec3& partitions);

   /////////////////////////////////////////////////////////////////////////////
   // Grouping
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
   // Spatial queries
   /////////////////////////////////////////////////////////////////////////////

   bool isVisible(const ViewFrustum& viewFrustum);
   bool isIntersect(BoundingBox& other);

   std::vector< BoundingGroup > getContainingBoundingGroups(
    BoundingBox* boundingBox) const;
   std::vector< Collision > getCollidingElements() const;
   std::vector< BoundingBox* > getVisibleElements(
    const ViewFrustum& viewFrustum) const;

private:
   BoundingBox _boundingBox;
   glm::ivec3 _partitions;
   std::vector< BoundingGroup > _boundingGroups;
   unsigned int _numBoundingBoxes;
};

} // namespace space
} // namespace crash
