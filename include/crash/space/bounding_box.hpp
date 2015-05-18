#pragma once

#include <array>
#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <crash/common/movable.hpp>

namespace crash {

namespace render {
   class ViewFrustum;
}

namespace space {

class BoundingBox : public common::Movable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Type definitions.
   /////////////////////////////////////////////////////////////////////////////

   static const int NUM_CORNERS = 8;
   static const int NUM_FACE_NORMALS = 3;
   static const int NUM_DIAGONALS = 4;

   typedef std::array< glm::vec3, NUM_CORNERS > Corners;
   typedef std::array< glm::vec3, NUM_FACE_NORMALS > FaceNormals;
   typedef std::array< glm::vec3, NUM_DIAGONALS > DiagonalDirections;

   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   BoundingBox(const BoundingBox& boundingBox);
   BoundingBox(const common::Transformer& transformer);

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
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   const common::Transformer& getTransformer() const;
   void setTransformer(const common::Transformer& transformer);

   /////////////////////////////////////////////////////////////////////////////
   // Memoization.
   /////////////////////////////////////////////////////////////////////////////

   void invalidate();

   /**
    * Calculate the radius of the sphere that circumscribes this BoundingBox.
    */
   float getRadius();

   /**
    * Calculate the corners of this BoundingBox.
    * Ordered:
    *    0: near bottom  left (nbl)
    *    1: near bottom right (nbr)
    *    2: near    top  left (ntl)
    *    3: near    top right (ntr)
    *    4:  far bottom  left (fbl)
    *    5:  far bottom right (fbr)
    *    6:  far    top  left (ftl)
    *    7:  far    top right (ftr)
    */
   const Corners& getCorners();

   /**
    * Calculate the normal vectors of the faces of this BoundingBox.
    * Ordered:
    *    0: right
    *    1: top
    *    2: far
    */
   const FaceNormals& getFaceNormals();

   /**
    * Calculate the diagonal direction vectors of this BoundingBox.
    * Ordered:
    *    0: nbl -> ftr
    *    1: nbr -> ftl
    *    2: ntl -> fbr
    *    3: ntr -> fbl
    */
   const DiagonalDirections& getDiagonalDirections();

   /////////////////////////////////////////////////////////////////////////////
   // Spatial queries.
   /////////////////////////////////////////////////////////////////////////////

   /**
    * Determine if any part of this BoundingBox is within the given ViewFrustum.
    *
    * :param viewFrustum:  The ViewFrustum to test this BoundingBox against.
    */
   bool isVisible(const render::ViewFrustum& viewFrustum);

   /**
    * Determine if this BoundingBox is intersecting with the specified other
    * BoundingBox.
    *
    * :param other: The other BoundingBox to test for intersection.
    */
   bool isIntersecting(BoundingBox& boundable);

private:
   // Data members
   common::Transformer _transformer;

   // Memoization
   void generateRadius();
   void generateCorners();
   void generateFaceNormals();
   void generateDiagonalDirections();

   boost::optional< float > _radius;
   boost::optional< Corners > _corners;
   boost::optional< FaceNormals > _faceNormals;
   boost::optional< DiagonalDirections > _diagonalDirections;

   // Spatial queries
   typedef std::array< float, 9 > CoefficientMatrix;

   /**
    * Calculate intersection between this BoundingBox and the other specified
    * BoundingBox using a sphere approximation.
    */
   bool intersectAsSpheres(BoundingBox& boundingBox);

   /**
    * Calculate intersection between this BoundingBox and the other specified
    * BoundingBox using method of separating axis.
    */
   bool intersectAsBoxes(BoundingBox& boundingBox);

   /**
    * Calculate the projection of the difference in box centers onto the
    * current axis.
    */
   static float centerDifferenceProjection(int aNdx, int bNdx,
    const glm::vec3& normalDots, CoefficientMatrix coefMatrix);

   /**
    * Calculate the projection of the intersection radius of this BoundingBox
    * onto the current axis.
    */
   static float thisIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& aSize, CoefficientMatrix absCoefMatrix);

   /**
    * Calculate the projection of the intersection radius of other BoundingBox
    * onto the current axis.
    */
   static float otherIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& bSize, CoefficientMatrix absCoefMatrix);

   int _frustumPlaneIndex;
};

} // namespace space
} // namespace crash
