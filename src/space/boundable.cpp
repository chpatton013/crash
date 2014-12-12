#include <cmath>
#include <crash/space/boundable.hpp>
#include <crash/math/math.hpp>

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
const Boundable::Corners& Boundable::getCorners() {
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
const Boundable::FaceNormals& Boundable::getFaceNormals() {
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
const Boundable::DiagonalDirections& Boundable::getDiagonalDirections() {
   if (!this->_diagonalDirections) {
      this->generateDiagonalDirections();
   }

   return this->_diagonalDirections.get();
 }

///////////////////////////////////////////////////////////////////////////////
// Spatial queries
///////////////////////////////////////////////////////////////////////////////

bool Boundable::isVisible(const ViewFrustum& viewFrustum) {
   auto corners = this->getCorners();
   auto diagonals = this->getDiagonalDirections();
   auto frustumPlanes = viewFrustum.getPlanes();

   for (int planeCount = 0; planeCount < NUM_VIEW_FRUSTUM_PLANES; ++planeCount) {
      // Utilize temporal locality by starting at the last failing plane.
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
      int closestDiagonal = 0;
      float closestProjection = std::abs(glm::dot(diagonals[0], plane.normal));
      for (int diagNdx = 1; diagNdx < NUM_DIAGONALS; ++diagNdx) {
         float projection = std::abs(glm::dot(diagonals[diagNdx], plane.normal));
         if (projection > closestProjection) {
            closestDiagonal = diagNdx;
            closestProjection = projection;
         }
      }

      // Check distance of each extreme point.
      // This step involves 1 or 2 dot products.
      auto c1 = corners[closestDiagonal];
      auto c2 = corners[(NUM_CORNERS - 1) - closestDiagonal];
      if (plane.distance(c1) < 0 && plane.distance(c2) < 0) {
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
   this->_radius = boost::none;
   this->_transform = boost::none;
   this->_corners = boost::none;
   this->_faceNormals = boost::none;
   this->_diagonalDirections = boost::none;
}

void Boundable::generateRadius() {
   this->_radius = std::sqrt(glm::dot(this->_size, this->_size));
}

void Boundable::generateTransform() {
   this->_transform = math::transform(this->_position, this->_orientation);
}

void Boundable::generateCorners() {
   static const std::array< glm::vec4, NUM_CORNERS > unitCorners {{
      glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), // 0:nbl
      glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), // 1:nbr
      glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), // 2:ntl
      glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), // 3:ntr
      glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), // 4:fbl
      glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), // 5:fbr
      glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), // 6:ftl
      glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), // 7:ftr
   }};

   auto transform = this->getTransform();

   Boundable::Corners corners;
   for (int ndx = 0; ndx < NUM_CORNERS; ++ndx) {
      corners[ndx] = glm::vec3(transform * unitCorners[ndx]);
   }

   this->_corners = corners;
}

void Boundable::generateFaceNormals() {
   auto corners = this->getCorners();

   Boundable::FaceNormals normals {{
      math::Plane::fromPoints(corners[5], corners[7], corners[3]).normal, // 0:right: fbr ftr ntr
      math::Plane::fromPoints(corners[3], corners[7], corners[6]).normal, // 1:top:   ntr ftr ftl
      math::Plane::fromPoints(corners[6], corners[7], corners[5]).normal, // 2:far:   ftl ftr fbr
   }};

   this->_faceNormals = normals;
}

void Boundable::generateDiagonalDirections() {
   auto corners = this->getCorners();

   Boundable::DiagonalDirections diagonals;
   for (int ndx = 0; ndx < NUM_DIAGONALS; ++ndx) {
      auto v1 = corners[ndx];
      auto v2 = corners[(NUM_CORNERS - 1) - ndx];
      diagonals[ndx] = glm::normalize(glm::vec3(v2 - v1));
   }

   this->_diagonalDirections = diagonals;
}

///////////////////////////////////////////////////////////////////////////////
// Intersection methods
///////////////////////////////////////////////////////////////////////////////

bool Boundable::intersectAsSpheres(Boundable& other) {
   float minimumDistance = this->getRadius() + other.getRadius();
   glm::vec3 centerDistance = this->_position - other._position;

   return math::approxGreaterThan(minimumDistance * minimumDistance,
    glm::dot(centerDistance, centerDistance));
}

bool Boundable::intersectAsBoxes(Boundable& other) {
   Boundable& a = *this;
   Boundable& b = other;

   bool parallelPairExists = false;

   glm::vec3 centerDistance = b.position() - a.position();

   glm::vec3 aSize = a.size() * 0.5f;
   glm::vec3 bSize = b.size() * 0.5f;

   auto aNormals = a.getFaceNormals();
   auto bNormals = b.getFaceNormals();

   glm::vec3 normalDots;
   CoefficientMatrix coefMatrix;
   CoefficientMatrix absCoefMatrix;

   // Assess face normals from this Boundable.
   // If a set of parallel axes exist we can avoid checking normal cross
   // products later on.
   for (int aNdx = 0; aNdx < 3; ++aNdx) {
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         float c = glm::dot(aNormals[aNdx], bNormals[bNdx]);
         float absC = std::abs(c);

         int index = crash::math::linearize_index(glm::ivec2(aNdx, bNdx),
          glm::ivec2(3, 3));

         coefMatrix[index] = c;
         absCoefMatrix[index] = absC;
         if (crash::math::approxGreaterThan(absC, 1.0f)) {
            parallelPairExists = true;
         }
      }

      float aDotD = glm::dot(aNormals[aNdx], centerDistance);
      normalDots[aNdx] = aDotD;

      float interval = std::abs(aDotD);
      float aRadius = aSize[aNdx];
      float bRadius = 0.0f;
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         int index = crash::math::linearize_index(glm::ivec2(aNdx, bNdx),
          glm::ivec2(3, 3));

         bRadius += bSize[bNdx] * absCoefMatrix[index];
      }

      if (crash::math::approxGreaterThan(interval, aRadius + bRadius)) {
         return parallelPairExists;
      }
   }

   // Assess face normals from other Boundable.
   // We don't need to look for parallel axes anymore.
   for (int bNdx = 0; bNdx < 3; ++bNdx) {
      float interval = std::abs(glm::dot(bNormals[bNdx], centerDistance));
      float aRadius = 0.0f;
      for (int aNdx = 0; aNdx < 3; ++aNdx) {
         int index = crash::math::linearize_index(glm::ivec2(aNdx, bNdx),
          glm::ivec2(3, 3));

         aRadius += aSize[aNdx] * absCoefMatrix[index];
      }
      float bRadius = bSize[bNdx];

      if (crash::math::approxGreaterThan(interval, aRadius + bRadius)) {
         return parallelPairExists;
      }
   }

   if (parallelPairExists) {
      return true;
   }

   // Assess axes generated from face normal cross products.
   for (int aNdx = 0; aNdx < 3; ++aNdx) {
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         float interval = Boundable::centerDifferenceProjection(
          aNdx, bNdx, normalDots, coefMatrix);
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

/* static */ float Boundable::centerDifferenceProjection(int aNdx, int bNdx,
 const glm::vec3& normalDots, CoefficientMatrix coefMatrix) {
   int aDotNdx = (aNdx + 2) % 3;
   int bDotNdx = (aNdx + 1) % 3;

   int aCoefIndex = crash::math::linearize_index(glm::ivec2(bDotNdx, bNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = crash::math::linearize_index(glm::ivec2(aDotNdx, bNdx),
    glm::ivec2(3, 3));

   float aCoef = coefMatrix[aCoefIndex];
   float bCoef = coefMatrix[bCoefIndex];

   return std::abs(normalDots[aDotNdx] * aCoef - normalDots[bDotNdx] * bCoef);
}

/* static */ float Boundable::thisIntersectionRadiusProjection(
 int aNdx, int bNdx, const glm::vec3& aSize, CoefficientMatrix absCoefMatrix) {
   int aSizeNdx = (aNdx + 1) % 3;
   int bSizeNdx = (aNdx + 2) % 3;

   int aCoefIndex = crash::math::linearize_index(glm::ivec2(bSizeNdx, bNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = crash::math::linearize_index(glm::ivec2(aSizeNdx, bNdx),
    glm::ivec2(3, 3));

   float aCoef = absCoefMatrix[aCoefIndex];
   float bCoef = absCoefMatrix[bCoefIndex];

   return aSize[aSizeNdx] * aCoef + aSize[bSizeNdx] * bCoef;
}

/* static */ float Boundable::otherIntersectionRadiusProjection(
 int aNdx, int bNdx, const glm::vec3& bSize, CoefficientMatrix absCoefMatrix) {
   int aSizeNdx = bNdx ? 0 : 1;
   int bSizeNdx = (bNdx == 2) ? 1 : 2;

   int aCoefIndex = crash::math::linearize_index(glm::ivec2(aNdx, bSizeNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = crash::math::linearize_index(glm::ivec2(aNdx, aSizeNdx),
    glm::ivec2(3, 3));

   float aCoef = absCoefMatrix[aCoefIndex];
   float bCoef = absCoefMatrix[bCoefIndex];

   return bSize[aSizeNdx] * aCoef + bSize[aSizeNdx] * bCoef;
}
