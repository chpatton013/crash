#include <crash/math/symbols.hpp>
#include <crash/space/bounding_partition.hpp>
#include <crash/space/util.hpp>

using namespace crash::math;
using namespace crash::space;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

BoundingPartition::BoundingPartition(const BoundingPartition& spatialManager) :
   _boundingBox(spatialManager._boundingBox),
   _partitions(spatialManager._partitions),
   _boundingGroups(spatialManager._boundingGroups),
   _numBoundingBoxes(spatialManager._numBoundingBoxes)
{}

BoundingPartition::BoundingPartition(const Transformer& transformer,
 const glm::ivec3& partitions) :
   _boundingBox(transformer, glm::vec3(), glm::vec4(xAxis, 0.0f)),
    _numBoundingBoxes(0)
{
   this->partition(transformer, partitions);
}

////////////////////////////////////////////////////////////////////////////////
// Transformable interface.
////////////////////////////////////////////////////////////////////////////////

const glm::vec3& BoundingPartition::getPosition() const {
   return this->_boundingBox.getTransformer().getPosition();
}

const glm::vec4& BoundingPartition::getOrientation() const {
   return this->_boundingBox.getTransformer().getOrientation();
}

const glm::vec3& BoundingPartition::getSize() const {
   return this->_boundingBox.getTransformer().getSize();
}

void BoundingPartition::setPosition(const glm::vec3& position) {
   Transformer transformer(this->_boundingBox.getTransformer());
   transformer.setPosition(position);
   this->resize(transformer, this->_partitions);
}

void BoundingPartition::setOrientation(const glm::vec4& orientation) {
   Transformer transformer(this->_boundingBox.getTransformer());
   transformer.setOrientation(orientation);
   this->resize(transformer, this->_partitions);
}

void BoundingPartition::setSize(const glm::vec3& size) {
   Transformer transformer(this->_boundingBox.getTransformer());
   transformer.setSize(size);
   this->resize(transformer, this->_partitions);
}

const glm::mat4& BoundingPartition::getTransform() {
   return this->_boundingBox.getTransform();
}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const BoundingBox& BoundingPartition::getBoundingBox() const {
   return this->_boundingBox;
}

void BoundingPartition::resize(const Transformer& transformer,
 const glm::ivec3& partitions) {
   std::vector< BoundingBox* > boundaingBoxes = this->getBoundingBoxes();

   this->partition(transformer, partitions);

   for (auto boundingBox : boundaingBoxes) {
      this->add(boundingBox);
   }
}

void BoundingPartition::partition(const Transformer& transformer,
 const glm::ivec3& partitions) {
   this->_numBoundingBoxes = 0;

   this->_boundingBox.setTransformer(transformer);
   this->_partitions = partitions;

   int totalPartitions = this->_partitions.x * this->_partitions.y *
    this->_partitions.z;
   this->_boundingGroups.clear();
   this->_boundingGroups.reserve(totalPartitions);

   auto size = transformer.getSize();
   glm::vec3 dimensions = glm::vec3(
      size.x / (float)partitions.x,
      size.y / (float)partitions.y,
      size.z / (float)partitions.z
   );

   auto position = transformer.getPosition();
   auto orientation = transformer.getOrientation();
   for (int ndx = 0; ndx < totalPartitions; ++ndx) {
      glm::ivec3 index = vectorize_index(ndx, this->_partitions);
      glm::vec3 center = position + glm::vec3(
         index.x * dimensions.x * 0.5f,
         index.y * dimensions.y * 0.5f,
         index.z * dimensions.z * 0.5f
      );
      this->_boundingGroups.push_back(BoundingGroup(Transformer(
       center, orientation, dimensions)));
   }
}

////////////////////////////////////////////////////////////////////////////////
// Grouping.
////////////////////////////////////////////////////////////////////////////////

bool BoundingPartition::add(BoundingBox* boundingBox) {
   bool added = false;

   for (auto& group : this->_boundingGroups) {
      if (group.isIntersecting(*boundingBox)) {
         added |= group.add(boundingBox);
      }
   }

   if (added) {
      ++this->_numBoundingBoxes;
   }

   return added;
}

bool BoundingPartition::remove(BoundingBox* boundingBox) {
   bool removed = false;

   for (auto& group : this->_boundingGroups) {
      removed |= group.remove(boundingBox);
   }

   if (removed) {
      --this->_numBoundingBoxes;
   }

   return removed;
}

bool BoundingPartition::update(BoundingBox* boundingBox) {
   if (!this->remove(boundingBox)) {
      return false;
   }

   return this->add(boundingBox);
}

void BoundingPartition::clear() {
   for (BoundingGroup& group : this->_boundingGroups) {
      group.clear();
   }

   this->_numBoundingBoxes = 0;
}

unsigned int BoundingPartition::getNumBoundingBoxes() const {
   return this->_numBoundingBoxes;
}

unsigned int BoundingPartition::getNumBoundingGroups() const {
   return this->_boundingGroups.size();
}

std::vector< BoundingBox* > BoundingPartition::getBoundingBoxes() const {
   std::vector< BoundingBox* > accumulator;
   std::set< BoundingBox* > mark;

   accumulator.reserve(this->_numBoundingBoxes);
   for (auto& group : this->_boundingGroups) {
      for (auto boundingBox : group.getBoundingBoxes()) {
         auto itr = mark.find(boundingBox);
         if (itr == mark.end()) {
            accumulator.push_back(boundingBox);
            mark.insert(boundingBox);
         }
      }
   }

   return accumulator;
}

std::vector< BoundingGroup > BoundingPartition::getBoundingGroups() const {
   return this->_boundingGroups;
}

const glm::ivec3& BoundingPartition::getPartitions() const {
   return this->_partitions;
}

////////////////////////////////////////////////////////////////////////////////
// Spatial queries.
////////////////////////////////////////////////////////////////////////////////

std::vector< BoundingGroup > BoundingPartition::getContainingBoundingGroups(
 BoundingBox* boundingBox) const {
   std::vector< BoundingGroup > groups;

   for (auto group : this->_boundingGroups) {
      if (group.isIntersecting(*boundingBox)) {
         groups.push_back(group);
      }
   }

   return groups;
}

std::vector< Collision > BoundingPartition::getCollidingElements() const {
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

std::vector< BoundingBox* > BoundingPartition::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< BoundingBox* > accumulator;
   std::set< BoundingBox* > mark;

   for (auto& group : this->_boundingGroups) {
      for (auto boundingBox : group.getVisibleElements(viewFrustum)) {
         auto itr = mark.find(boundingBox);
         if (itr == mark.end()) {
            accumulator.push_back(boundingBox);
            mark.insert(boundingBox);
         }
      }
   }

   return accumulator;
}
