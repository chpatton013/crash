#pragma once

#include <array>
#include <glm/glm.hpp>
#include <boost/optional.hpp>
#include <crash/math/transformer.hpp>

namespace crash {
namespace space {

class ViewFrustum;

/**
 * The data type used to represent a drawable and/or intersectable object.
 *
 * :member position:    The absolute global position of the Boundable.
 * :member orientation: The direction the Boundable is facing.
 * :member size:        The size of the Boundable.
 * :member velocity:    The linear velocity of the Boundable.
 */
class Boundable {
public:
   static const int NUM_CORNERS = 8;
   static const int NUM_FACE_NORMALS = 3;
   static const int NUM_DIAGONALS = 4;

   typedef std::array< glm::vec3, Boundable::NUM_CORNERS > Corners;
   typedef std::array< glm::vec3, Boundable::NUM_FACE_NORMALS > FaceNormals;
   typedef std::array< glm::vec3, Boundable::NUM_DIAGONALS > DiagonalDirections;

   Boundable(const Boundable& boundable);
   Boundable(const math::Transformer& transformer, const glm::vec3& velocity);

   ////////////////////////////////////////////////////////////////////////////
   // Data access
   ////////////////////////////////////////////////////////////////////////////

   const math::Transformer& transformer() const;
   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   const glm::vec3& size() const;
   const glm::vec3& velocity() const;

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void size(const glm::vec3& size);
   void velocity(const glm::vec3& velocity);

   ////////////////////////////////////////////////////////////////////////////
   // Memoization
   ////////////////////////////////////////////////////////////////////////////

   void invalidate();

   /**
    * Calculate the radius of the sphere that circumscribes this Boundable.
    */
   float getRadius();

   /**
    * Calculate the transform matrix of this Boundable.
    */
   const glm::mat4& getTransform();

   /**
    * Calculate the corners of the box enclosing this Boundable.
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
   const Boundable::Corners& getCorners();

   /**
    * Calculate the normal vectors of the faces of the box enclosing this
    * Boundable.
    * Ordered:
    *    0: right
    *    1: top
    *    2: far
    */
   const Boundable::FaceNormals& getFaceNormals();

   /**
    * Calculate the diagonal direction vectors of this Boundable.
    * Ordered:
    *    0: nbl -> ftr
    *    1: nbr -> ftl
    *    2: ntl -> fbr
    *    3: ntr -> fbl
    */
   const Boundable::DiagonalDirections& getDiagonalDirections();

   ////////////////////////////////////////////////////////////////////////////
   // Spatial queries
   ////////////////////////////////////////////////////////////////////////////

   /**
    * Determine if any part of this Boundable is within the given view frustum.
    *
    * :param viewFrustum:  The ViewFrustum to test this Boundable against.
    */
   bool isVisible(const ViewFrustum& viewFrustum);

   /**
    * Determine if this Boundable is intersecting with the specified other
    * Boundable. Intersection is assessed with a sphere approximation, then with
    * oriented bounding boxes.
    *
    * :param other: The other Boundable to test for intersection.
    */
   bool intersect(Boundable& other);

private:
   // Data members
   math::Transformer _transformer;
   glm::vec3 _velocity;

   // Memoization
   void generateRadius();
   void generateCorners();
   void generateFaceNormals();
   void generateDiagonalDirections();

   boost::optional< float > _radius;
   boost::optional< Boundable::Corners > _corners;
   boost::optional< Boundable::FaceNormals > _faceNormals;
   boost::optional< Boundable::DiagonalDirections > _diagonalDirections;

   // Spatial queries
   typedef std::array< float, 9 > CoefficientMatrix;

   /**
    * Calculate intersection between this Boundable and the other specified
    * Boundable using a sphere approximation.
    */
   bool intersectAsSpheres(Boundable& other);

   /**
    * Calculate intersection between this Boundable and the other specified
    * Boundable using method of separating axis.
    */
   bool intersectAsBoxes(Boundable& other);

   /**
    * Calculate the projection of the difference in box centers onto the
    * current axis.
    */
   static float centerDifferenceProjection(int aNdx, int bNdx,
    const glm::vec3& normalDots, CoefficientMatrix coefMatrix);

   /**
    * Calculate the projection of the intersection radius of this Boundable
    * onto the current axis.
    */
   static float thisIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& aSize, CoefficientMatrix absCoefMatrix);

   /**
    * Calculate the projection of the intersection radius of other Boundable
    * onto the current axis.
    */
   static float otherIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& bSize, CoefficientMatrix absCoefMatrix);

   int _frustumPlaneIndex;
};

} // namespace space
} // namespace crash
