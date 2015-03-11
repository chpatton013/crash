#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/collision.hpp>

namespace crash {
namespace space {

class ViewFrustum;

class SpatialManager {
public:
   SpatialManager(const SpatialManager& manager);
   SpatialManager(const math::Transformer& transformer,
    const glm::ivec3& partitions);

   ////////////////////////////////////////////////////////////////////////////
   // Data access
   ////////////////////////////////////////////////////////////////////////////

   const Boundable& boundable() const;
   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   const glm::vec3& size() const;

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void size(const glm::vec3& size);

   ////////////////////////////////////////////////////////////////////////////
   // Grouping
   ////////////////////////////////////////////////////////////////////////////

   bool add(Boundable* boundable);
   bool remove(Boundable* boundable);
   bool update(Boundable* boundable);
   void clear();

   unsigned int getBoundableCount() const;
   unsigned int getBoundingGroupCount() const;

   const glm::ivec3& getPartitions() const;
   void resize(const math::Transformer& transformer,
    const glm::ivec3& partitions);

   std::vector< Boundable* > getBoundables() const;
   const std::vector< BoundingGroup >& getBoundingGroups() const;

   ////////////////////////////////////////////////////////////////////////////
   // Spatial queries
   ////////////////////////////////////////////////////////////////////////////

   bool isVisible(const ViewFrustum& viewFrustum);
   bool intersect(Boundable& other);

   std::vector< BoundingGroup > getContainingGroups(Boundable* boundable) const;
   std::vector< Collision > getCollidingElements() const;
   std::vector< Boundable* > getVisibleElements(const ViewFrustum& viewFrustum)
    const;

private:
   void rePartition(const math::Transformer& transformer,
    const glm::ivec3& partitions);

   Boundable _boundable;
   glm::ivec3 _partitions;
   std::vector< BoundingGroup > _boundingGroups;
   unsigned int _numBoundables;
};

} // namespace space
} // namespace crash
