#include <crash/math/math.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/spatial_manager.hpp>

using namespace crash::space;

SpatialManager::SpatialManager(const glm::vec3& dimensions,
 const glm::ivec3& partitions) :
   numBoundables(0)
{
   this->partition(dimensions, partitions);
}

/* virtual */ SpatialManager::~SpatialManager() {
   this->deleteGroups();
}

void SpatialManager::resize(const glm::vec3& dimensions,
 const glm::ivec3& partitions) {
   auto boundables = this->getBoundables();

   this->deleteGroups();
   this->partition(dimensions, partitions);

   for (auto boundable : boundables) {
      this->add(boundable);
   }
}

bool SpatialManager::add(Boundable* boundable) {
   bool added = false;

   for (auto group : this->boundingGroups) {
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

   for (auto group : this->boundingGroups) {
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
   for (auto group : this->boundingGroups) {
      group.clear();
   }

   this->numBoundables = 0;
}

const glm::vec3& SpatialManager::getDimensions() const {
   return this->dimensions;
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

void SpatialManager::partition(const glm::vec3& dimensions,
 const glm::ivec3& partitions) {
   this->dimensions = dimensions;
   this->partitions = partitions;

   int totalPartitions = this->partitions.x * this->partitions.y *
    this->partitions.z;
   this->boundingGroups.reserve(totalPartitions);

   glm::vec3 partDims = glm::vec3(
      dimensions.x / (float)partitions.x,
      dimensions.y / (float)partitions.y,
      dimensions.z / (float)partitions.z
   );
   glm::vec3 partCenter = partDims * 0.5f;

   for (int ndx = 0; ndx < totalPartitions; ++ndx) {
      glm::ivec3 index = crash::math::vectorize_index(ndx, this->partitions);
      glm::vec3 center = partCenter + glm::vec3(
         index.x * partDims.x,
         index.y * partDims.y,
         index.z * partDims.z
      );
      this->boundingGroups.push_back(new BoundingGroup(center, partDims));
   }
}

void SpatialManager::deleteGroups() {
   this->boundingGroups.clear();
   this->numBoundables = 0;
}
