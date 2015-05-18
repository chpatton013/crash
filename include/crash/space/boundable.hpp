#pragma once

namespace crash {

namespace render {
   class ViewFrustum;
}

namespace space {

class BoundingBox;

struct Boundable {
   virtual BoundingBox& getBoundingBox() = 0;

   /**
    * Determine if any part of this Boundable is within the given ViewFrustum.
    *
    * :param viewFrustum:  The ViewFrustum to test this BoundingBox against.
    */
   virtual bool isVisible(const render::ViewFrustum& viewFrustum) = 0;

   /**
    * Determine if this BoundingBox is intersecting with the specified other
    * Boundable.
    *
    * :param other: The other Boundable to test for intersection.
    */
   virtual bool isIntersecting(Boundable* boundable) = 0;
};

} // namespace space
} // namespace crash
