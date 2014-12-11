#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/collision.hpp>
#include <crash/space/view_frustum.hpp>

namespace crash {
namespace space {

class SpatialManager {
public:
   SpatialManager(const glm::vec3& dimensions, const glm::ivec3& partitions);
   virtual ~SpatialManager();

   void resize(const glm::vec3& dimensions, const glm::ivec3& partitions);
   bool add(Boundable* boundable);
   bool remove(Boundable* boundable);
   void clear();

   const glm::vec3& getDimensions() const;
   const glm::ivec3& getPartitions() const;

   unsigned int getBoundableCount() const;
   unsigned int getBoundingGroupCount() const;

   std::vector< Boundable* > getBoundables() const;
   std::vector< BoundingGroup > getBoundingGroups() const;
   std::vector< BoundingGroup > getContainingGroups(Boundable* boundable) const;

   std::vector< Collision > getCollisionQueue() const;
   std::vector< Boundable* > getRenderQueue(const ViewFrustum& viewFrustum) const;

protected:
   void partition(const glm::vec3& dimensions, const glm::ivec3& partitions);
   void deleteGroups();

   glm::vec3 dimensions;
   glm::ivec3 partitions;
   std::vector< BoundingGroup > boundingGroups;
   int numBoundables;
};

} // namespace space
} // namespace crash
