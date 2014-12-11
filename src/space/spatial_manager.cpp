#include <crash/math/math.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/spatial_manager.hpp>

using namespace crash::space;

SpatialManager::SpatialManager(const glm::vec3& size,
 const glm::ivec3& partitions) :
   SpatialManager(crash::math::origin, size, partitions)
{}

SpatialManager::SpatialManager(const glm::vec3& position, const glm::vec3& size,
 const glm::ivec3& partitions) :
   SpatialManager(position, glm::vec4(crash::math::xAxis, 0.0f), size,
    partitions)
{}

SpatialManager::SpatialManager(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size,
 const glm::ivec3& partitions) :
   numBoundables(0)
{
   this->partition(position, orientation, size, partitions);
}

void SpatialManager::resize(const glm::vec3& size,
 const glm::ivec3& partitions) {
   this->resize(crash::math::origin, size, partitions);
}

void SpatialManager::resize(const glm::vec3& position, const glm::vec3& size,
 const glm::ivec3& partitions) {
   this->resize(position, glm::vec4(crash::math::xAxis, 0.0f), size,
    partitions);
}

void SpatialManager::resize(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size,
 const glm::ivec3& partitions) {
   auto boundables = this->getBoundables();

   this->partition(position, orientation, size, partitions);

   for (auto boundable : boundables) {
      this->add(boundable);
   }
}

bool SpatialManager::add(Boundable* boundable) {
   bool added = false;

   for (auto& group : this->boundingGroups) {
      if (boundable->intersect(group)) {
         group.add(boundable);
         added = true;
      }
   }

   if (added) {
      ++this->numBoundables;
   }

   return added;
}

bool SpatialManager::remove(Boundable* boundable) {
   bool removed = false;

   for (auto& group : this->boundingGroups) {
      removed |= group.remove(boundable);
   }

   if (removed) {
      --this->numBoundables;
   }

   return removed;
}

bool SpatialManager::update(Boundable* boundable) {
   if (!this->remove(boundable)) {
      return false;
   }

   return this->add(boundable);
}

void SpatialManager::clear() {
   for (auto& group : this->boundingGroups) {
      group.clear();
   }

   this->numBoundables = 0;
}

const glm::vec3& SpatialManager::getSize() const {
   return this->size;
}

const glm::ivec3& SpatialManager::getPartitions() const {
   return this->partitions;
}

unsigned int SpatialManager::getBoundableCount() const {
   return this->numBoundables;
}

unsigned int SpatialManager::getBoundingGroupCount() const {
   return this->boundingGroups.size();
}

std::vector< Boundable* > SpatialManager::getBoundables() const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   accumulator.reserve(this->numBoundables);
   for (auto group : this->boundingGroups) {
      for (auto boundable : group.getBoundables()) {
         auto itr = mark.find(boundable);
         if (itr == mark.end()) {
            accumulator.push_back(boundable);
            mark.insert(boundable);
         }
      }
   }

   return accumulator;
}

std::vector< BoundingGroup > SpatialManager::getBoundingGroups() const {
   return this->boundingGroups;
}

std::vector< BoundingGroup > SpatialManager::getContainingGroups(
 Boundable* boundable) const {
   std::vector< BoundingGroup > groups;

   for (auto group : this->boundingGroups) {
      if (boundable->intersect(group)) {
         groups.push_back(group);
      }
   }

   return groups;
}

std::vector< Collision > SpatialManager::getCollisionQueue() const {
   std::vector< Collision > accumulator;
   std::set< Collision > mark;

   for (auto group : this->boundingGroups) {
      for (auto collision : group.getCollidingElements()) {
         auto itr = mark.find(collision);
         if (itr == mark.end()) {
            accumulator.push_back(collision);
            mark.insert(collision);
         }
      }
   }

   return accumulator;
}

std::vector< Boundable* > SpatialManager::getRenderQueue(
 const ViewFrustum& viewFrustum) const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   for (auto group : this->boundingGroups) {
      for (auto boundable : group.getVisibleElements(viewFrustum)) {
         auto itr = mark.find(boundable);
         if (itr == mark.end()) {
            accumulator.push_back(boundable);
            mark.insert(boundable);
         }
      }
   }

   return accumulator;
}

void SpatialManager::partition(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size,
 const glm::ivec3& partitions) {
   this->numBoundables = 0;

   this->position = position;
   this->orientation = orientation;
   this->size = size;
   this->partitions = partitions;

   int totalPartitions = this->partitions.x * this->partitions.y *
    this->partitions.z;
   this->boundingGroups.clear();
   this->boundingGroups.reserve(totalPartitions);

   glm::vec3 dimensions = glm::vec3(
      size.x / (float)partitions.x,
      size.y / (float)partitions.y,
      size.z / (float)partitions.z
   );

   for (int ndx = 0; ndx < totalPartitions; ++ndx) {
      glm::ivec3 index = crash::math::vectorize_index(ndx, this->partitions);
      glm::vec3 center = position + glm::vec3(
         index.x * dimensions.x * 0.5f,
         index.y * dimensions.y * 0.5f,
         index.z * dimensions.z * 0.5f
      );
      this->boundingGroups.push_back(BoundingGroup(center, orientation,
       dimensions));
   }
}
