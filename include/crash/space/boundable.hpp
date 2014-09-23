#pragma once

#include <array>
#include <glm/glm.hpp>
#include <crash/space/camera.hpp>

namespace crash {
namespace space {

const int NUM_BOUNDABLE_CORNERS = 8;
const int NUM_BOUNDABLE_FACE_NORMALS = 3;
const int NUM_BOUNDABLE_DIAGONALS = 4;

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
   Boundable();
   Boundable(const glm::vec3& position);
   Boundable(const glm::vec3& position, const glm::vec4& orientation);
   Boundable(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size);
   Boundable(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size, const glm::vec3& velocity);

   ////////////////////////////////////////////////////////////////////////////
   // Data getters
   ////////////////////////////////////////////////////////////////////////////

   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   const glm::vec3& size() const;
   const glm::vec3& velocity() const;

   ////////////////////////////////////////////////////////////////////////////
   // Data setters
   ////////////////////////////////////////////////////////////////////////////

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void size(const glm::vec3& size);
   void velocity(const glm::vec3& velocity);

   ////////////////////////////////////////////////////////////////////////////
   // Memoized getters
   ////////////////////////////////////////////////////////////////////////////

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
   const std::array< glm::vec3, NUM_BOUNDABLE_CORNERS >& getCorners();

   /**
    * Calculate the normal vectors of the faces of the box enclosing this
    * Boundable.
    * Ordered:
    *    0: right
    *    1: top
    *    2: far
    */
   const std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS >& getFaceNormals();

   /**
    * Calculate the diagonal direction vectors of this Boundable.
    * Ordered:
    *    0: nbl -> ftr
    *    1: nbr -> ftl
    *    2: ntl -> fbr
    *    3: ntr -> fbl
    */
   const std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS >&
    getDiagonalDirections();

   ////////////////////////////////////////////////////////////////////////////
   // Other methods
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
   glm::vec3 _position;
   glm::vec4 _orientation;
   glm::vec3 _size;
   glm::vec3 _velocity;

   // Memoization generators
   void invalidate();
   void generateRadius();
   void generateTransform();
   void generateCorners();
   void generateFaceNormals();
   void generateDiagonalDirections();

   // Memoization members
   boost::optional< float > _radius;
   boost::optional< glm::mat4 > _transform;
   boost::optional< std::array< glm::vec3, NUM_BOUNDABLE_CORNERS > > _corners;
   boost::optional< std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS > >
    _faceNormals;
   boost::optional< std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS > >
    _diagonalDirections;

   struct intersection_data_t {
      glm::vec3 aSize;
      glm::vec3 bSize;
      glm::vec3 normalDots;
      std::array< std::array< float, 3 >, 3 > coefMatrix;
      std::array< std::array< float, 3 >, 3 > absCoefMatrix;
   };
   typedef std::tuple< boost::optional< intersection_data_t >, bool >
    intersection_data_generation_t;

   // Utility methods

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
    * Generate data needed for intersection calculations. Look for conditions
    * to short-circuit intersection calculation early.
    */
   static intersection_data_generation_t
    initializeIntersectionData(Boundable& a, Boundable& b);

   /**
    * Calculate the projection of the difference in box centers onto the
    * current axis.
    */
   static float centerDifferenceProjection(int aNdx, int bNdx,
    const glm::vec3& normalDots,
    std::array< std::array< float, 3 >, 3 > coefMatrix);

   /**
    * Calculate the projection of the intersection radius of this Boundable
    * onto the current axis.
    */
   static float thisIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& aSize,
    std::array< std::array< float, 3 >, 3 > absCoefMatrix);

   /**
    * Calculate the projection of the intersection radius of other Boundable
    * onto the current axis.
    */
   static float otherIntersectionRadiusProjection(int aNdx, int bNdx,
    const glm::vec3& bSize,
    std::array< std::array< float, 3 >, 3 > absCoefMatrix);

   // Utility members
   int _frustumPlaneIndex;
};

} // namespace space
} // namespace crash
