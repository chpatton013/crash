#include <crash/math/symbols.hpp>
#include <crash/math/transformer.hpp>
#include <crash/space/bounding_group.hpp>
#include <crash/space/view_frustum.hpp>

using namespace crash::math;
using namespace crash::space;

////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////

BoundingGroup::BoundingGroup(const BoundingGroup& boundingGroup) :
   _boundingBox(boundingGroup._boundingBox),
   _boundingBoxes(boundingGroup._boundingBoxes)
{}

BoundingGroup::BoundingGroup(const Transformer& transformer) :
   _boundingBox(transformer, glm::vec3(), glm::vec4(xAxis, 0.0f))
{}

////////////////////////////////////////////////////////////////////////////////
// Grouping
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
// Spatial queries
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
