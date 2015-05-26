#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/common/transformer.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/bounding_group.hpp>

namespace crash {

namespace render {
   class ViewFrustum;
}

namespace space {

class BoundingPartition;
typedef std::shared_ptr< BoundingPartition > BoundingPartitionPtr;

class Collision;

class BoundingPartition : public Boundable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingPartition(const BoundingPartition& spatialManager);
   BoundingPartition(const common::Transformer& transformer,
    const glm::ivec3& partitions);

   /////////////////////////////////////////////////////////////////////////////
   // Movable interface.
   /////////////////////////////////////////////////////////////////////////////

   glm::vec3 getPosition() const;
   glm::quat getOrientation() const;
   glm::vec3 getSize() const;
   glm::vec3 getTranslationalVelocity() const;
   glm::quat getRotationalVelocity() const;
   glm::vec3 getScaleVelocity() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::quat& orientation);
   void setSize(const glm::vec3& size);
   void setTranslationalVelocity(const glm::vec3& translationalVelocity);
   void setRotationalVelocity(const glm::quat& rotationalVelocity);
   void setScaleVelocity(const glm::vec3& scaleVelocity);

   /////////////////////////////////////////////////////////////////////////////
   // Boundable interface.
   /////////////////////////////////////////////////////////////////////////////

   BoundingBox* getBoundingBox();

   /////////////////////////////////////////////////////////////////////////////
   // Grouping.
   /////////////////////////////////////////////////////////////////////////////

   void resize(const common::Transformer& transformer,
    const glm::ivec3& partitions);
   void partition(const common::Transformer& transformer,
    const glm::ivec3& partitions);

   bool add(Boundable* boundable);
   bool remove(Boundable* boundable);
   bool update(Boundable* boundable);
   void clear();

   unsigned int getNumBoundables() const;
   unsigned int getNumBoundingGroups() const;

   std::vector< Boundable* > getBoundables() const;
   std::vector< BoundingGroup > getBoundingGroups() const;

   const glm::ivec3& getPartitions() const;

   std::vector< BoundingGroup > getContainingBoundingGroups(
    Boundable* boundingBox) const;
   std::vector< Collision > getCollidingElements() const;
   std::vector< Boundable* > getVisibleElements(
    const render::ViewFrustum& viewFrustum) const;

private:
   BoundingBox _boundingBox;
   glm::ivec3 _partitions;
   std::vector< BoundingGroup > _boundingGroups;
   unsigned int _numBoundables;
};

} // namespace space
} // namespace crash
