#pragma once

#include <set>
#include <vector>
#include <crash/space/boundable.hpp>
#include <crash/space/view_frustum.hpp>
#include <crash/space/collision.hpp>

namespace crash {
namespace space {

class BoundingGroup : public Boundable {
public:
   BoundingGroup(const glm::vec3& position, const glm::vec3& scale);

   void add(Boundable* boundable);
   bool remove(Boundable* boundable);
   void clear();
   const std::set< Boundable* >& getBoundables() const;

   std::vector< Collision > getCollidingElements() const;
   std::vector< Boundable* > getVisibleElements(const ViewFrustum& viewFrustum)
    const;

protected:
   std::set< Boundable* > _boundables;
};

} // namespace space
} // namespace crash
