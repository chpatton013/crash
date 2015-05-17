#include <crash/math/symbols.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/view_frustum.hpp>

using namespace crash::math;
using namespace crash::space;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

BoundingGroup::BoundingGroup(const BoundingGroup& boundingGroup) :
   _boundingBox(boundingGroup._boundingBox),
   _boundingBoxes(boundingGroup._boundingBoxes)
{}

BoundingGroup::BoundingGroup(const Transformer& transformer) :
   _boundingBox(transformer)
{}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 BoundingGroup::getPosition() const {
   return this->_boundingBox.getPosition();
}

glm::quat BoundingGroup::getOrientation() const {
   return this->_boundingBox.getOrientation();
}

glm::vec3 BoundingGroup::getSize() const {
   return this->_boundingBox.getSize();
}

glm::vec3 BoundingGroup::getTranslationalVelocity() const {
   return this->_boundingBox.getTranslationalVelocity();
}

glm::quat BoundingGroup::getRotationalVelocity() const {
   return this->_boundingBox.getRotationalVelocity();
}

glm::vec3 BoundingGroup::getScaleVelocity() const {
   return this->_boundingBox.getScaleVelocity();
}

void BoundingGroup::setPosition(const glm::vec3& position) {
   this->_boundingBox.setPosition(position);
}

void BoundingGroup::setOrientation(const glm::quat& orientation) {
   this->_boundingBox.setOrientation(orientation);
}

void BoundingGroup::setSize(const glm::vec3& size) {
   this->_boundingBox.setSize(size);
}

void BoundingGroup::setTranslationalVelocity(
 const glm::vec3& translationalVelocity) {
   this->_boundingBox.setTranslationalVelocity(translationalVelocity);
}

void BoundingGroup::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->_boundingBox.setRotationalVelocity(rotationalVelocity);
}

void BoundingGroup::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_boundingBox.setScaleVelocity(scaleVelocity);
}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const BoundingBox& BoundingGroup::getBoundingBox() const {
   return this->_boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
// Grouping.
////////////////////////////////////////////////////////////////////////////////

bool BoundingGroup::add(BoundingBox* boundingBox) {
   return this->_boundingBoxes.insert(boundingBox).second;
}

bool BoundingGroup::remove(BoundingBox* boundingBox) {
   auto itr = this->_boundingBoxes.find(boundingBox);
   if (itr == this->_boundingBoxes.end()) {
      return false;
   }

   this->_boundingBoxes.erase(itr);
   return true;
}

void BoundingGroup::clear() {
   this->_boundingBoxes.clear();
}

const std::set< BoundingBox* >& BoundingGroup::getBoundingBoxes() const {
   return this->_boundingBoxes;
}

////////////////////////////////////////////////////////////////////////////////
// Spatial queries.
////////////////////////////////////////////////////////////////////////////////

bool BoundingGroup::isVisible(const ViewFrustum& viewFrustum) {
   return this->_boundingBox.isVisible(viewFrustum);
}

bool BoundingGroup::isIntersecting(BoundingBox& other) {
   return this->_boundingBox.isIntersecting(other);
}

std::vector< Collision > BoundingGroup::getCollidingElements() const {
   std::vector< Collision > queue;
   auto begin = this->_boundingBoxes.begin();
   auto end = this->_boundingBoxes.end();

   for (auto itrA = begin; itrA != end; ++itrA) {
      // Start searching at the position after itrA so each combination of
      // BoundingBoxes is only checked once.
      auto itrB = itrA;
      while (++itrB != end) {
         auto a = *itrA;
         auto b = *itrB;

         if (a->isIntersecting(*b)) {
            queue.push_back(Collision::factory(a, b));
         }
      }
   }

   return queue;
}

std::vector< BoundingBox* > BoundingGroup::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< BoundingBox* > queue;

   for (auto boundingBox : this->_boundingBoxes) {
      if (boundingBox->isVisible(viewFrustum)) {
         queue.push_back(boundingBox);
      }
   }

   return queue;
}
