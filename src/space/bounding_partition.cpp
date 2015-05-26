#include <crash/common/symbols.hpp>
#include <crash/space/bounding_partition.hpp>
#include <crash/space/util.hpp>

using namespace crash::common;
using namespace crash::space;
using namespace crash::render;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

BoundingPartition::BoundingPartition(const BoundingPartition& spatialManager) :
   _boundingBox(spatialManager._boundingBox),
    _partitions(spatialManager._partitions),
    _boundingGroups(spatialManager._boundingGroups),
    _numBoundables(spatialManager._numBoundables)
{}

BoundingPartition::BoundingPartition(const Transformer& transformer,
 const glm::ivec3& partitions) :
   _boundingBox(transformer), _numBoundables(0)
{
   this->partition(transformer, partitions);
}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 BoundingPartition::getPosition() const {
   return this->_boundingBox.getPosition();
}

glm::quat BoundingPartition::getOrientation() const {
   return this->_boundingBox.getOrientation();
}

glm::vec3 BoundingPartition::getSize() const {
   return this->_boundingBox.getSize();
}

glm::vec3 BoundingPartition::getTranslationalVelocity() const {
   return this->_boundingBox.getTranslationalVelocity();
}

glm::quat BoundingPartition::getRotationalVelocity() const {
   return this->_boundingBox.getRotationalVelocity();
}

glm::vec3 BoundingPartition::getScaleVelocity() const {
   return this->_boundingBox.getScaleVelocity();
}

void BoundingPartition::setPosition(const glm::vec3& position) {
   this->_boundingBox.setPosition(position);
}

void BoundingPartition::setOrientation(const glm::quat& orientation) {
   this->_boundingBox.setOrientation(orientation);
}

void BoundingPartition::setSize(const glm::vec3& size) {
   this->_boundingBox.setSize(size);
}

void BoundingPartition::setTranslationalVelocity(
 const glm::vec3& translationalVelocity) {
   this->_boundingBox.setTranslationalVelocity(translationalVelocity);
}

void BoundingPartition::setRotationalVelocity(
 const glm::quat& rotationalVelocity) {
   this->_boundingBox.setRotationalVelocity(rotationalVelocity);
}

void BoundingPartition::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_boundingBox.setScaleVelocity(scaleVelocity);
}

////////////////////////////////////////////////////////////////////////////////
// Boundable interface.
////////////////////////////////////////////////////////////////////////////////

BoundingBox* BoundingPartition::getBoundingBox() {
   return &this->_boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
// Grouping.
////////////////////////////////////////////////////////////////////////////////

void BoundingPartition::resize(const Transformer& transformer,
 const glm::ivec3& partitions) {
   std::vector< Boundable* > boundables = this->getBoundables();

   this->partition(transformer, partitions);

   for (Boundable* boundable : boundables) {
      this->add(boundable);
   }
}

void BoundingPartition::partition(const Transformer& transformer,
 const glm::ivec3& partitions) {
   this->_numBoundables = 0;

   this->_boundingBox.setTransformer(transformer);
   this->_partitions = partitions;

   unsigned int totalPartitions = this->_partitions.x * this->_partitions.y *
    this->_partitions.z;
   this->_boundingGroups.clear();
   this->_boundingGroups.reserve(totalPartitions);

   glm::vec3 size = transformer.getSize();
   glm::vec3 dimensions = glm::vec3(
      size.x / (float)partitions.x,
      size.y / (float)partitions.y,
      size.z / (float)partitions.z
   );

   glm::vec3 corner = transformer.getPosition() - (size * 0.5f);
   glm::quat orientation = transformer.getOrientation();
   for (unsigned int ndx = 0; ndx < totalPartitions; ++ndx) {
      glm::ivec3 index = vectorize_index(ndx, this->_partitions);
      glm::vec3 relativePosition = glm::vec3(
         index.x * dimensions.x,
         index.y * dimensions.y,
         index.z * dimensions.z
      );
      glm::vec3 position = corner + (dimensions * 0.5f) + relativePosition;

      this->_boundingGroups.push_back(BoundingGroup(Transformer(
       position, orientation, dimensions,
       glm::vec3(), NO_ROTATION, glm::vec3())));
   }
}

bool BoundingPartition::add(Boundable* boundingBox) {
   this->_boundingBoxes.erase(boundingBox);

   bool added = false;

   for (BoundingGroup& group : this->_boundingGroups) {
      if (group.isIntersecting(boundingBox)) {
         added |= group.add(boundingBox);
      }
   }

   if (!added) {
      this->_boundingBoxes.insert(boundingBox);
   }

   ++this->_numBoundables;
   return added;
}

bool BoundingPartition::remove(Boundable* boundingBox) {
   bool removed = false;

   for (BoundingGroup& group : this->_boundingGroups) {
      removed |= group.remove(boundingBox);
   }

   if (!removed) {
      auto itr = this->_boundingBoxes.find(boundingBox);
      if (itr != this->_boundingBoxes.end()) {
         this->_boundingBoxes.erase(itr);
         removed = true;
      }
   }

   if (removed) {
      --this->_numBoundables;
   }

   return removed;
}

bool BoundingPartition::update(Boundable* boundable) {
   this->remove(boundable);
   return this->add(boundable);
}

void BoundingPartition::clear() {
   for (BoundingGroup& group : this->_boundingGroups) {
      group.clear();
   }

   this->_boundingBoxes.clear();

   this->_numBoundables = 0;
}

unsigned int BoundingPartition::getNumBoundables() const {
   return this->_numBoundables;
}

unsigned int BoundingPartition::getNumBoundingGroups() const {
   return this->_boundingGroups.size();
}

std::vector< Boundable* > BoundingPartition::getBoundables() const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   accumulator.reserve(this->_numBoundables);

   for (Boundable* boundable : this->_boundingBoxes) {
      auto itr = mark.find(boundable);
      if (itr == mark.end()) {
         accumulator.push_back(boundable);
         mark.insert(boundable);
      }
   }

   for (const BoundingGroup& group : this->_boundingGroups) {
      for (Boundable* boundable : group.getBoundables()) {
         auto itr = mark.find(boundable);
         if (itr == mark.end()) {
            accumulator.push_back(boundable);
            mark.insert(boundable);
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

std::vector< BoundingGroup > BoundingPartition::getContainingBoundingGroups(
 Boundable* boundingBox) const {
   std::vector< BoundingGroup > groups;

   for (auto group : this->_boundingGroups) {
      if (group.isIntersecting(boundingBox)) {
         groups.push_back(group);
      }
   }

   return groups;
}

std::vector< Collision > BoundingPartition::getCollidingElements() const {
   std::vector< Collision > accumulator;
   std::set< Collision > mark;

   for (const BoundingGroup& group : this->_boundingGroups) {
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

std::vector< Boundable* > BoundingPartition::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< Boundable* > accumulator;
   std::set< Boundable* > mark;

   for (Boundable* boundable : this->_boundingBoxes) {
      auto itr = mark.find(boundable);
      if (itr == mark.end() && boundable->isVisible(viewFrustum)) {
         accumulator.push_back(boundable);
         mark.insert(boundable);
      }
   }

   for (const BoundingGroup& group : this->_boundingGroups) {
      for (Boundable* boundingBox : group.getVisibleElements(viewFrustum)) {
         auto itr = mark.find(boundingBox);
         if (itr == mark.end()) {
            accumulator.push_back(boundingBox);
            mark.insert(boundingBox);
         }
      }
   }

   return accumulator;
}
