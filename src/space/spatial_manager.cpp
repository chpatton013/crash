#include <crash/math/math.hpp>
#include <crash/math/plane.hpp>
#include <crash/space/spatial_manager.hpp>
#include <crash/space/view_frustum.hpp>

using namespace crash::space;

SpatialManager::SpatialManager(const SpatialManager& manager) :
   SpatialManager(manager._boundable.transformer(), manager._partitions)
{}

SpatialManager::SpatialManager(const math::Transformer& transformer,
 const glm::ivec3& partitions) :
   _boundable(transformer, glm::vec3()), _partitions(partitions),
    _numBoundables(0)
{
   this->rePartition(transformer, partitions);
}

///////////////////////////////////////////////////////////////////////////////
// Data access
///////////////////////////////////////////////////////////////////////////////

const Boundable& SpatialManager::boundable() const {
   return this->_boundable;
}

const glm::vec3& SpatialManager::position() const {
   return this->_boundable.position();
}

const glm::vec4& SpatialManager::orientation() const {
   return this->_boundable.orientation();
}

const glm::vec3& SpatialManager::size() const {
   return this->_boundable.size();
}

void SpatialManager::position(const glm::vec3& position) {
   this->_boundable.position(position);
}

void SpatialManager::orientation(const glm::vec4& orientation) {
   this->_boundable.orientation(orientation);
}

void SpatialManager::size(const glm::vec3& size) {
   this->_boundable.size(size);
}

///////////////////////////////////////////////////////////////////////////////
// Grouping
///////////////////////////////////////////////////////////////////////////////

bool SpatialManager::add(Boundable* boundable) {
   bool added = false;

   for (auto& group : this->_boundingGroups) {
      if (group.intersect(*boundable)) {
         added |= group.add(boundable);
      }
   }

   if (added) {
      ++this->_numBoundables;
   }

   return added;
}

bool SpatialManager::remove(Boundable* boundable) {
   bool removed = false;

   for (auto& group : this->_boundingGroups) {
      removed |= group.remove(boundable);
   }

   if (removed) {
      --this->_numBoundables;
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
   for (auto& group : this->_boundingGroups) {
      group.clear();
   }

   this->_numBoundables = 0;
}

unsigned int SpatialManager::getBoundableCount() const {
   return this->_numBoundables;
}

unsigned int SpatialManager::getBoundingGroupCount() const {
   return this->_boundingGroups.size();
}

const glm::ivec3& SpatialManager::getPartitions() const {
   return this->_partitions;
}

void SpatialManager::resize(const math::Transformer& transformer,
 const glm::ivec3& partitions) {
   auto boundables = this->getBoundables();

   this->rePartition(transformer, partitions);

   for (auto boundable : boundables) {
      this->add(boundable);
   }
}

std::vector< Boundable* > SpatialManager::getBoundables() const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   accumulator.reserve(this->_numBoundables);
   for (auto& group : this->_boundingGroups) {
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

const std::vector< BoundingGroup >& SpatialManager::getBoundingGroups() const {
   return this->_boundingGroups;
}

///////////////////////////////////////////////////////////////////////////////
// Spatial queries
///////////////////////////////////////////////////////////////////////////////

std::vector< BoundingGroup > SpatialManager::getContainingGroups(
 Boundable* boundable) const {
   std::vector< BoundingGroup > groups;

   for (auto group : this->_boundingGroups) {
      if (group.intersect(*boundable)) {
         groups.push_back(group);
      }
   }

   return groups;
}

std::vector< Collision > SpatialManager::getCollidingElements() const {
   std::vector< Collision > accumulator;
   std::set< Collision > mark;

   for (auto& group : this->_boundingGroups) {
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

std::vector< Boundable* > SpatialManager::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   for (auto group : this->_boundingGroups) {
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

void SpatialManager::rePartition(const math::Transformer& transformer,
 const glm::ivec3& partitions) {
   glm::vec3 position = transformer.position();
   glm::vec4 orientation = transformer.orientation();
   glm::vec3 size = transformer.size();

   this->_numBoundables = 0;

   this->position(position);
   this->orientation(orientation);
   this->size(size);
   this->_partitions = partitions;

   int totalPartitions = this->_partitions.x * this->_partitions.y *
    this->_partitions.z;
   this->_boundingGroups.clear();
   this->_boundingGroups.reserve(totalPartitions);

   glm::vec3 dimensions = glm::vec3(
      size.x / (float)partitions.x,
      size.y / (float)partitions.y,
      size.z / (float)partitions.z
   );

   for (int ndx = 0; ndx < totalPartitions; ++ndx) {
      glm::ivec3 index = crash::math::vectorize_index(ndx, this->_partitions);
      glm::vec3 center = position + glm::vec3(
         index.x * dimensions.x * 0.5f,
         index.y * dimensions.y * 0.5f,
         index.z * dimensions.z * 0.5f
      );
      this->_boundingGroups.push_back(BoundingGroup(math::Transformer(
       center, orientation, dimensions)));
   }
}
