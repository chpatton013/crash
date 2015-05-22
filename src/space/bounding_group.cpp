#include <crash/common/symbols.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/render/view_frustum.hpp>

using namespace crash::common;
using namespace crash::space;
using namespace crash::render;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

BoundingGroup::BoundingGroup(const BoundingGroup& boundingGroup) :
   _boundingBox(boundingGroup._boundingBox),
    _boundables(boundingGroup._boundables)
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
// Boundable interface.
////////////////////////////////////////////////////////////////////////////////

BoundingBox* BoundingGroup::getBoundingBox() {
   return &this->_boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
// Grouping.
////////////////////////////////////////////////////////////////////////////////

bool BoundingGroup::add(Boundable* boundingBox) {
   return this->_boundables.insert(boundingBox).second;
}

bool BoundingGroup::remove(Boundable* boundingBox) {
   auto itr = this->_boundables.find(boundingBox);
   if (itr == this->_boundables.end()) {
      return false;
   }

   this->_boundables.erase(itr);
   return true;
}

void BoundingGroup::clear() {
   this->_boundables.clear();
}

const std::set< Boundable* >& BoundingGroup::getBoundables() const {
   return this->_boundables;
}

std::vector< Collision > BoundingGroup::getCollidingElements() const {
   std::vector< Collision > queue;
   auto begin = this->_boundables.begin();
   auto end = this->_boundables.end();

   for (auto itrA = begin; itrA != end; ++itrA) {
      // Start searching at the position after itrA so each combination of
      // Boundables is only checked once.
      auto itrB = itrA;
      while (++itrB != end) {
         auto a = *itrA;
         auto b = *itrB;

         if (a->isIntersecting(b)) {
            queue.push_back(Collision::factory(a, b));
         }
      }
   }

   return queue;
}

std::vector< Boundable* > BoundingGroup::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< Boundable* > queue;

   for (Boundable* boundable : this->_boundables) {
      if (boundable->isVisible(viewFrustum)) {
         queue.push_back(boundable);
      }
   }

   return queue;
}
