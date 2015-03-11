#include <crash/math/math.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/view_frustum.hpp>

using namespace crash::space;

BoundingGroup::BoundingGroup(const BoundingGroup& boundingGroup) :
   BoundingGroup(boundingGroup._boundable.transformer())
{}

BoundingGroup::BoundingGroup(const math::Transformer& transformer) :
   _boundable(transformer, glm::vec3())
{}

///////////////////////////////////////////////////////////////////////////////
// Data access
///////////////////////////////////////////////////////////////////////////////

const Boundable& BoundingGroup::boundable() const {
   return this->_boundable;
}

const glm::vec3& BoundingGroup::position() const {
   return this->_boundable.position();
}

const glm::vec4& BoundingGroup::orientation() const {
   return this->_boundable.orientation();
}

const glm::vec3& BoundingGroup::size() const {
   return this->_boundable.size();
}

void BoundingGroup::position(const glm::vec3& position) {
   this->_boundable.position(position);
}

void BoundingGroup::orientation(const glm::vec4& orientation) {
   this->_boundable.orientation(orientation);
}

void BoundingGroup::size(const glm::vec3& size) {
   this->_boundable.size(size);
}

///////////////////////////////////////////////////////////////////////////////
// Grouping
///////////////////////////////////////////////////////////////////////////////

bool BoundingGroup::add(Boundable* boundable) {
   return this->_boundables.insert(boundable).second;
}

bool BoundingGroup::remove(Boundable* boundable) {
   auto itr = this->_boundables.find(boundable);
   if (itr != this->_boundables.end()) {
      this->_boundables.erase(itr);
      return true;
   }

   return false;
}

void BoundingGroup::clear() {
   this->_boundables.clear();
}

const std::set< Boundable* >& BoundingGroup::getBoundables() const {
   return this->_boundables;
}

///////////////////////////////////////////////////////////////////////////////
// Spatial queries
///////////////////////////////////////////////////////////////////////////////

bool BoundingGroup::isVisible(const ViewFrustum& viewFrustum) {
   return this->_boundable.isVisible(viewFrustum);
}

bool BoundingGroup::intersect(Boundable& other) {
   return this->_boundable.intersect(other);
}

std::vector< Collision > BoundingGroup::getCollidingElements() const {
   std::vector< Collision > queue;
   auto begin = this->_boundables.begin();
   auto end = this->_boundables.end();

   for (auto itrA = begin; itrA != end; ++itrA) {
      auto itrB = itrA;

      while (++itrB != end) {
         auto a = *itrA;
         auto b = *itrB;

         if (a->intersect(*b)) {
            queue.push_back(Collision::factory(a, b));
         }
      }
   }

   return queue;
}

std::vector< Boundable* > BoundingGroup::getVisibleElements(
 const ViewFrustum& viewFrustum) const {
   std::vector< Boundable* > queue;

   for (auto boundable : this->_boundables) {
      if (boundable->isVisible(viewFrustum)) {
         queue.push_back(boundable);
      }
   }

   return queue;
}
