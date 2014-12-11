#include <crash/math/math.hpp>
#include <crash/space/bounding_group.hpp>

using namespace crash::space;

BoundingGroup::BoundingGroup(const glm::vec3& size) :
   BoundingGroup(crash::math::origin, glm::vec4(crash::math::xAxis, 0.0f), size)
{}

BoundingGroup::BoundingGroup(const glm::vec3& position,
 const glm::vec3& size) :
   BoundingGroup(position, glm::vec4(crash::math::xAxis, 0.0f), size)
{}

BoundingGroup::BoundingGroup(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size) :
   Boundable(position, orientation, size)
{}

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
