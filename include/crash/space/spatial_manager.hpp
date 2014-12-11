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
   SpatialManager(const glm::vec3& size, const glm::ivec3& partitions);
   SpatialManager(const glm::vec3& position, const glm::vec3& size,
    const glm::ivec3& partitions);
   SpatialManager(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size, const glm::ivec3& partitions);

   void resize(const glm::vec3& size, const glm::ivec3& partitions);
   void resize(const glm::vec3& position, const glm::vec3& size,
    const glm::ivec3& partitions);
   void resize(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size, const glm::ivec3& partitions);

   bool add(Boundable* boundable);
   bool remove(Boundable* boundable);
   bool update(Boundable* boundable);
   void clear();

   const glm::vec3& getSize() const;
   const glm::ivec3& getPartitions() const;

   unsigned int getBoundableCount() const;
   unsigned int getBoundingGroupCount() const;

   std::vector< Boundable* > getBoundables() const;
   std::vector< BoundingGroup > getBoundingGroups() const;
   std::vector< BoundingGroup > getContainingGroups(Boundable* boundable) const;

   std::vector< Collision > getCollisionQueue() const;
   std::vector< Boundable* > getRenderQueue(const ViewFrustum& viewFrustum) const;

protected:
   void partition(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size, const glm::ivec3& partitions);
   void deleteGroups();

   glm::vec3 position;
   glm::vec4 orientation;
   glm::vec3 size;
   glm::ivec3 partitions;
   std::vector< BoundingGroup > boundingGroups;
   unsigned int numBoundables;
};

} // namespace space
} // namespace crash
