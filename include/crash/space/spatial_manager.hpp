#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/collision.hpp>
#include <crash/space/view_frustum.hpp>

namespace crash {
namespace space {

class SpatialManager : public Boundable {
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

   const glm::ivec3& getPartitions() const;

   unsigned int getBoundableCount() const;
   unsigned int getBoundingGroupCount() const;

   std::vector< Boundable* > getBoundables() const;
   std::vector< BoundingGroup > getBoundingGroups() const;
   std::vector< BoundingGroup > getContainingGroups(Boundable* boundable) const;

   std::vector< Collision > getCollisionQueue() const;
   std::vector< Boundable* > getRenderQueue(const ViewFrustum& viewFrustum) const;

private:
   void partition(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size, const glm::ivec3& partitions);

   glm::ivec3 _partitions;
   std::vector< BoundingGroup > _boundingGroups;
   unsigned int _numBoundables;
};

} // namespace space
} // namespace crash
