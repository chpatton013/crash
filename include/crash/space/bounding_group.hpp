#pragma once

#include <set>
#include <vector>
#include <crash/space/boundable.hpp>
#include <crash/space/collision.hpp>

namespace crash {

namespace math {
   class Transformer;
}

namespace space {

class ViewFrustum;

class BoundingGroup {
public:
   BoundingGroup(const BoundingGroup& boundingGroup);
   BoundingGroup(const math::Transformer& transformer);

   ////////////////////////////////////////////////////////////////////////////
   // Data access
   ////////////////////////////////////////////////////////////////////////////

   const Boundable& boundable() const;
   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   const glm::vec3& size() const;

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void size(const glm::vec3& size);

   ////////////////////////////////////////////////////////////////////////////
   // Grouping
   ////////////////////////////////////////////////////////////////////////////

   bool add(Boundable* boundable);
   bool remove(Boundable* boundable);
   void clear();
   const std::set< Boundable* >& getBoundables() const;

   ////////////////////////////////////////////////////////////////////////////
   // Spatial queries
   ////////////////////////////////////////////////////////////////////////////

   bool isVisible(const ViewFrustum& viewFrustum);
   bool intersect(Boundable& other);

   std::vector< Collision > getCollidingElements() const;
   std::vector< Boundable* > getVisibleElements(const ViewFrustum& viewFrustum)
    const;

protected:
   Boundable _boundable;
   std::set< Boundable* > _boundables;
};

} // namespace space
} // namespace crash
