#include <crash/space/boundable.hpp>
#include <crash/math/math.hpp>
#include <crash/util/type.hpp>

using namespace crash::space;

Boundable::Boundable() :
   Boundable(glm::vec3(0.0f))
{}

Boundable::Boundable(const glm::vec3& position) :
   Boundable(position, glm::vec4(crash::math::xAxis, 0.0f))
{}

Boundable::Boundable(const glm::vec3& position, const glm::vec4& orientation) :
   Boundable(position, orientation, glm::vec3(1.0f))
{}

Boundable::Boundable(const glm::vec3& position, const glm::vec4& orientation,
 const glm::vec3& size) :
   Boundable(position, orientation, size, glm::vec3(0.0f))
{}

Boundable::Boundable(const glm::vec3& position, const glm::vec4& orientation,
 const glm::vec3& size, const glm::vec3& velocity) :
   _position(position), _orientation(orientation), _size(size),
    _velocity(velocity), _frustumPlaneIndex(0)
{}

///////////////////////////////////////////////////////////////////////////////
// Data getters
///////////////////////////////////////////////////////////////////////////////

const glm::vec3& Boundable::position() const { return this->_position; }
const glm::vec4& Boundable::orientation() const { return this->_orientation; }
const glm::vec3& Boundable::size() const { return this->_size; }
const glm::vec3& Boundable::velocity() const { return this->_velocity; }

///////////////////////////////////////////////////////////////////////////////
// Data setters
///////////////////////////////////////////////////////////////////////////////

void Boundable::position(const glm::vec3& position) {
   this->invalidate();
   this->_position = position;
}

void Boundable::orientation(const glm::vec4& orientation) {
   this->invalidate();
   this->_orientation = orientation;
}

void Boundable::size(const glm::vec3& size) {
   this->invalidate();
   this->_size = size;
}

void Boundable::velocity(const glm::vec3& velocity) {
   this->invalidate();
   this->_velocity = velocity;
}

///////////////////////////////////////////////////////////////////////////////
// Memoized getters
///////////////////////////////////////////////////////////////////////////////

/**
 * Calculate the radius of the sphere that circumscribes this Boundable.
 */
float Boundable::getRadius() {
   if (!this->_radius) {
      this->generateRadius();
   }

   return this->_radius.get();
}

/**
 * Calculate the transform matrix of this Boundable.
 */
const glm::mat4& Boundable::getTransform() {
   if (!this->_transform) {
      this->generateTransform();
   }

   return this->_transform.get();
}

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
const std::array< glm::vec3, NUM_BOUNDABLE_CORNERS >& Boundable::getCorners() {
   if (!this->_corners) {
      this->generateCorners();
   }

   return this->_corners.get();
}

/**
 * Calculate the normal vectors of the faces of the box enclosing this
 * Boundable.
 * Ordered:
 *    0: right
 *    1: top
 *    2: far
 */
const std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS >&
 Boundable::getFaceNormals() {
   if (!this->_faceNormals) {
      this->generateFaceNormals();
   }

   return this->_faceNormals.get();
}

/**
 * Calculate the diagonal direction vectors of this Boundable.
 * Ordered:
 *    0: nbl -> ftr
 *    1: nbr -> ftl
 *    2: ntl -> fbr
 *    3: ntr -> fbl
 */
const std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS >&
 Boundable::getDiagonalDirections() {
   if (!this->_diagonalDirections) {
      this->generateDiagonalDirections();
   }

   return this->_diagonalDirections.get();
 }

///////////////////////////////////////////////////////////////////////////////
// Other methods
///////////////////////////////////////////////////////////////////////////////

bool Boundable::isVisible(const ViewFrustum& viewFrustum) {
   CAUTO corners = this->getCorners();
   CAUTO diagonals = this->getDiagonalDirections();
   CAUTO frustumPlanes = viewFrustum.getPlanes();

   // Utilize temporal locality by starting at the last failing plane.
   for (int planeCount = 0; planeCount < NUM_VIEW_FRUSTUM_PLANES; ++planeCount) {
      int planeNdx = (this->_frustumPlaneIndex + planeCount) %
       NUM_VIEW_FRUSTUM_PLANES;
      const math::Plane& plane = frustumPlanes[planeNdx];

      // Find the diagonal that is most orthogonal to the plane.
      // Doing this allows us to reduce the maximum number of dot products that
      // need to be calculated from 8 to 6. The minimum rises from 1 to 5, but
      // the reduction in worst-case behaviour results in better performance
      // when most Boundables are out of the viewing frustum - as is the norm
      // in large scenes where performance may start to suffer.
      // This step involves 4 dot products.
      int closestDiagonal = -1;
      float closestProjection = -1.0f;
      for (int diagNdx = 0; diagNdx < NUM_BOUNDABLE_DIAGONALS; ++diagNdx) {
         float projection = std::abs(glm::dot(diagonals[diagNdx], plane.normal));
         if (projection > closestProjection) {
            closestDiagonal = diagNdx;
            closestProjection = projection;
         }
      }

      // Check distance of each extreme point.
      // This step involves 1 or 2 dot products.
      CAUTO v1 = corners[closestDiagonal];
      CAUTO v2 = corners[(NUM_BOUNDABLE_CORNERS - 1) - closestDiagonal];
      if (plane.distance(v1) < 0 && plane.distance(v2) < 0) {
         this->_frustumPlaneIndex = planeNdx;
         return false;
      }
   }

   return true;
}

bool Boundable::intersect(Boundable& other) {
   return this->intersectAsSpheres(other) && this->intersectAsBoxes(other);
}

///////////////////////////////////////////////////////////////////////////////
// Memoization generators
///////////////////////////////////////////////////////////////////////////////

void Boundable::invalidate() {
   this->_radius = boost::optional< float >();
   this->_transform = boost::optional< glm::mat4 >();
   this->_corners = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_CORNERS > >();
   this->_faceNormals = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS > >();
   this->_diagonalDirections = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS > >();
}

void Boundable::generateRadius() {
   float radius = math::fastSqrt(glm::dot(this->_size, this->_size));
   this->_radius = boost::optional< float >(radius);
}

void Boundable::generateTransform() {
   glm::mat4 transform = math::transform(this->_position, this->_orientation);
   this->_transform = boost::optional< glm::mat4 >(transform);
}

void Boundable::generateCorners() {
   static const std::array< glm::vec4, NUM_BOUNDABLE_CORNERS > unitCorners {{
      glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
      glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f),
      glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f),
      glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f),
      glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f),
      glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f),
      glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f),
      glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f)
   }};

   std::array< glm::vec3, NUM_BOUNDABLE_CORNERS > corners;

   CAUTO transform = this->getTransform();
   for (int ndx = 0; ndx < NUM_BOUNDABLE_CORNERS; ++ndx) {
      corners[ndx] = glm::vec3(transform * unitCorners[ndx]);
   }

   this->_corners = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_CORNERS > >(corners);
}

void Boundable::generateFaceNormals() {
   CAUTO corners = this->getCorners();

   std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS> normals {{
      math::Plane::fromPoints(corners[3], corners[1], corners[5]).normal, // 1:right
      math::Plane::fromPoints(corners[2], corners[3], corners[7]).normal, // 2:top
      math::Plane::fromPoints(corners[4], corners[6], corners[7]).normal, // 5:far
   }};

   this->_faceNormals = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_FACE_NORMALS > >(normals);
}

void Boundable::generateDiagonalDirections() {
   CAUTO corners = this->getCorners();

   std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS > diagonals;
   for (int ndx = 0; ndx < NUM_BOUNDABLE_DIAGONALS; ++ndx) {
      CAUTO v1 = corners[ndx];
      CAUTO v2 = corners[(NUM_BOUNDABLE_CORNERS - 1) - ndx];
      diagonals[ndx] = glm::normalize(glm::vec3(v2 - v1));
   }

   this->_diagonalDirections = boost::optional<
    std::array< glm::vec3, NUM_BOUNDABLE_DIAGONALS > >(diagonals);
}

///////////////////////////////////////////////////////////////////////////////
// Utility methods
///////////////////////////////////////////////////////////////////////////////

bool Boundable::intersectAsSpheres(Boundable& other) {
   float interval = this->getRadius() + other.getRadius();
   glm::vec3 centerDistance = this->_position - other._position;

   return math::approxGreaterThan(interval * interval,
    glm::dot(centerDistance, centerDistance));
}

bool Boundable::intersectAsBoxes(Boundable& other) {
   intersection_data_generation_t intersectionDataGen =
    Boundable::initializeIntersectionData(*this, other);

   boost::optional< Boundable::intersection_data_t > intersectionDataOpt =
    std::get<0>(intersectionDataGen);
   if (!intersectionDataOpt) {
      return false;
   }

   bool existsParallelPair = std::get<1>(intersectionDataGen);
   if (existsParallelPair) {
      return true;
   }

   // Unpack generated data.
   Boundable::intersection_data_t intersectionData = intersectionDataOpt.get();
   glm::vec3 aSize = intersectionData.aSize;
   glm::vec3 bSize = intersectionData.bSize;
   glm::vec3 normalDots = intersectionData.normalDots;
   std::array< std::array< float, 3 >, 3 > coefMatrix =
    intersectionData.coefMatrix;
   std::array< std::array< float, 3 >, 3 > absCoefMatrix =
    intersectionData.absCoefMatrix;

   // Assess axes generated from face normal cross products.
   for (int aNdx = 0; aNdx < 3; ++aNdx) {
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         float interval = Boundable::centerDifferenceProjection(aNdx, bNdx,
          normalDots, coefMatrix);
         float aRadius = Boundable::thisIntersectionRadiusProjection(
          aNdx, bNdx, aSize, absCoefMatrix);
         float bRadius = Boundable::otherIntersectionRadiusProjection(
          aNdx, bNdx, bSize, absCoefMatrix);

         if (crash::math::approxGreaterThan(interval, aRadius + bRadius)) {
            return false;
         }
      }
   }

   return true;
}

// static
std::tuple< boost::optional< Boundable::intersection_data_t >, bool >
 Boundable::initializeIntersectionData(Boundable& a, Boundable& b) {
   bool existsParallelPair = false;

   glm::vec3 centerDistance = b.position() - a.position();

   glm::vec3 aSize = a.size() * 0.5f;
   glm::vec3 bSize = b.size() * 0.5f;

   CAUTO aNormals = a.getFaceNormals();
   CAUTO bNormals = b.getFaceNormals();

   glm::vec3 normalDots;
   std::array< std::array< float, 3 >, 3 > coefMatrix;
   std::array< std::array< float, 3 >, 3 > absCoefMatrix;

   // Assess face normals from this Boundable.
   // If a set of parallel axes exist we can avoid checking normal cross
   // products later on.
   for (int aNdx = 0; aNdx < 3; ++aNdx) {
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         float c = glm::dot(aNormals[aNdx], bNormals[bNdx]);
         float absC = std::abs(c);

         coefMatrix[aNdx][bNdx] = c;
         absCoefMatrix[aNdx][bNdx] = absC;
         if (crash::math::approxGreaterThan(absC, 1.0f)) {
            existsParallelPair = true;
         }
      }

      float aDotD = glm::dot(aNormals[aNdx], centerDistance);
      normalDots[aNdx] = aDotD;

      float interval = std::abs(aDotD);
      float aRadius = aSize[aNdx];
      float bRadius = 0.0f;
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         bRadius += bSize[bNdx] * absCoefMatrix[aNdx][bNdx];
      }

      if (crash::math::approxGreaterThan(interval, aRadius + bRadius)) {
         boost::optional< intersection_data_t > none;
         return intersection_data_generation_t(none, existsParallelPair);
      }
   }

   // Assess face normals from other Boundable.
   // We don't need to look for parallel axes anymore.
   for (int bNdx = 0; bNdx < 3; ++bNdx) {
      float interval = std::abs(glm::dot(bNormals[bNdx], centerDistance));
      float aRadius = 0.0f;
      for (int aNdx = 0; aNdx < 3; ++aNdx) {
         aRadius += aSize[aNdx] * absCoefMatrix[aNdx][bNdx];
      }
      float bRadius = bSize[bNdx];

      if (crash::math::approxGreaterThan(interval, aRadius + bRadius)) {
         boost::optional< intersection_data_t > none;
         return intersection_data_generation_t(none, existsParallelPair);
      }
   }

   boost::optional< intersection_data_t > intersectionDataOpt =
    boost::optional< intersection_data_t >({
      aSize, bSize, normalDots, coefMatrix, absCoefMatrix,
   });
   return intersection_data_generation_t(intersectionDataOpt, existsParallelPair);
}

// static
float Boundable::centerDifferenceProjection(int aNdx, int bNdx,
 const glm::vec3& normalDots,
 std::array< std::array< float, 3 >, 3 > coefMatrix) {
   int aCoefXNdx, aCoefYNdx;
   int bCoefXNdx, bCoefYNdx;
   int aDotNdx, bDotNdx;

   aDotNdx = bCoefXNdx = (aNdx + 2) % 3;
   bDotNdx = aCoefXNdx = (aNdx + 1) % 3;
   aCoefYNdx = bCoefYNdx = bNdx;

   float aCoef = coefMatrix[aCoefXNdx][aCoefYNdx];
   float bCoef = coefMatrix[bCoefXNdx][bCoefYNdx];

   return std::abs(normalDots[aDotNdx] * aCoef - normalDots[bDotNdx] * bCoef);
}

// static
float Boundable::thisIntersectionRadiusProjection(int aNdx, int bNdx,
 const glm::vec3& aSize,
 std::array< std::array< float, 3 >, 3 > absCoefMatrix) {
   int aCoefXNdx, aCoefYNdx;
   int bCoefXNdx, bCoefYNdx;
   int aSizeNdx, bSizeNdx;

   aSizeNdx = bCoefXNdx = (aNdx + 1) % 3;
   bSizeNdx = aCoefXNdx = (aNdx + 2) % 3;
   aCoefYNdx = bCoefYNdx = bNdx;

   float aCoef = absCoefMatrix[aCoefXNdx][aCoefYNdx];
   float bCoef = absCoefMatrix[bCoefXNdx][bCoefYNdx];

   return aSize[aSizeNdx] * aCoef + aSize[bSizeNdx] * bCoef;
}

// static
float Boundable::otherIntersectionRadiusProjection(int aNdx, int bNdx,
 const glm::vec3& bSize,
 std::array< std::array< float, 3 >, 3 > absCoefMatrix) {
   int aCoefXNdx, aCoefYNdx;
   int bCoefXNdx, bCoefYNdx;
   int aSizeNdx, bSizeNdx;

   aSizeNdx = bCoefYNdx = bNdx ? 0 : 1;
   bSizeNdx = aCoefYNdx = (bNdx == 2) ? 1 : 2;
   aCoefXNdx = bCoefXNdx = aNdx;

   float aCoef = absCoefMatrix[aCoefXNdx][aCoefYNdx];
   float bCoef = absCoefMatrix[bCoefXNdx][bCoefYNdx];

   return bSize[aSizeNdx] * aCoef + bSize[aSizeNdx] * bCoef;
}
