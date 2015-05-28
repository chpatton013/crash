#include <crash/common/plane.hpp>
#include <crash/common/arithmetic.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/util.hpp>
#include <crash/render/view_frustum.hpp>

using namespace crash::common;
using namespace crash::space;
using namespace crash::render;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox(const BoundingBox& boundingBox) :
   _transformer(boundingBox._transformer),
    _radius(boost::none), _corners(boost::none),
    _faceNormals(boost::none), _diagonalDirections(boost::none),
    _frustumPlaneIndex(0)
{}

BoundingBox::BoundingBox(const Transformer& transformer) :
   _transformer(transformer),
    _radius(boost::none), _corners(boost::none),
    _faceNormals(boost::none), _diagonalDirections(boost::none),
    _frustumPlaneIndex(0)
{}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 BoundingBox::getPosition() const {
   return this->_transformer.getPosition();
}

glm::quat BoundingBox::getOrientation() const {
   return this->_transformer.getOrientation();
}

glm::vec3 BoundingBox::getSize() const {
   return this->_transformer.getSize();
}

glm::vec3 BoundingBox::getTranslationalVelocity() const {
   return this->_transformer.getTranslationalVelocity();
}

glm::quat BoundingBox::getRotationalVelocity() const {
   return this->_transformer.getRotationalVelocity();
}

glm::vec3 BoundingBox::getScaleVelocity() const {
   return this->_transformer.getScaleVelocity();
}

void BoundingBox::setPosition(const glm::vec3& position) {
   this->_transformer.setPosition(position);
   this->invalidate();
}

void BoundingBox::setOrientation(const glm::quat& orientation) {
   this->_transformer.setOrientation(orientation);
   this->invalidate();
}

void BoundingBox::setSize(const glm::vec3& size) {
   this->_transformer.setSize(size);
   this->invalidate();
}

void BoundingBox::setTranslationalVelocity(
 const glm::vec3& translationalVelocity) {
   this->_transformer.setTranslationalVelocity(translationalVelocity);
   this->invalidate();
}

void BoundingBox::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->_transformer.setRotationalVelocity(rotationalVelocity);
   this->invalidate();
}

void BoundingBox::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_transformer.setScaleVelocity(scaleVelocity);
   this->invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// Boundable interface.
////////////////////////////////////////////////////////////////////////////////

BoundingBox* BoundingBox::getBoundingBox() {
   return this;
}

bool BoundingBox::isVisible(const ViewFrustum& viewFrustum) {
   auto corners = this->getCorners();
   auto diagonals = this->getDiagonalDirections();
   auto frustumPlanes = viewFrustum.getPlanes();

   for (unsigned int planeCount = 0; planeCount < ViewFrustum::NUM_PLANES;
    ++planeCount) {
      // Utilize temporal locality by starting at the last failing plane.
      unsigned int planeNdx = (this->_frustumPlaneIndex + planeCount) %
       ViewFrustum::NUM_PLANES;
      const Plane& plane = frustumPlanes[planeNdx];

      // Find the diagonal that is most orthogonal to the plane.
      // Doing this allows us to reduce the maximum number of dot products that
      // need to be calculated from 8 to 6. The minimum rises from 1 to 5, but
      // the reduction in worst-case behavior results in better performance
      // when most BoundingBoxes are out of the viewing frustum - as is the norm
      // in large scenes where performance may start to suffer.
      // This step involves 4 dot products.
      unsigned int closestDiagonal = 0;
      float closestProjection = std::abs(glm::dot(diagonals[0], plane.normal));
      for (unsigned int diagNdx = 1; diagNdx < NUM_DIAGONALS; ++diagNdx) {
         float projection = std::abs(glm::dot(diagonals[diagNdx], plane.normal));
         if (projection > closestProjection) {
            closestDiagonal = diagNdx;
            closestProjection = projection;
         }
      }

      // Check distance of each extreme point.
      // This step involves 1 or 2 dot products.
      const glm::vec3& c1 = corners[closestDiagonal];
      const glm::vec3& c2 = corners[(NUM_CORNERS - 1) - closestDiagonal];
      if (plane.distance(c1) < 0 && plane.distance(c2) < 0) {
         this->_frustumPlaneIndex = planeNdx;
         return false;
      }
   }

   return true;
}

bool BoundingBox::isIntersecting(Boundable* boundable) {
   BoundingBox* boundingBox = boundable->getBoundingBox();
   return this->intersectAsSpheres(*boundingBox) &&
    this->intersectAsBoxes(*boundingBox);
}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const Transformer& BoundingBox::getTransformer() const {
   return this->_transformer;
}

void BoundingBox::setTransformer(const Transformer& transformer) {
   this->_transformer = transformer;
   this->invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// Memoization.
////////////////////////////////////////////////////////////////////////////////

void BoundingBox::invalidate() {
   this->_radius = boost::none;
   this->_corners = boost::none;
   this->_faceNormals = boost::none;
   this->_diagonalDirections = boost::none;
}

float BoundingBox::getRadius() {
   if (!this->_radius) {
      this->generateRadius();
   }

   return this->_radius.get();
}

const BoundingBox::Corners& BoundingBox::getCorners() {
   if (!this->_corners) {
      this->generateCorners();
   }

   return this->_corners.get();
}

const BoundingBox::FaceNormals& BoundingBox::getFaceNormals() {
   if (!this->_faceNormals) {
      this->generateFaceNormals();
   }

   return this->_faceNormals.get();
}

const BoundingBox::DiagonalDirections& BoundingBox::getDiagonalDirections() {
   if (!this->_diagonalDirections) {
      this->generateDiagonalDirections();
   }

   return this->_diagonalDirections.get();
}

void BoundingBox::generateRadius() {
   this->_radius = std::sqrt(glm::dot(this->getSize(), this->getSize()));
}

void BoundingBox::generateCorners() {
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

   glm::mat4 transform = this->getTransform();

   Corners corners;
   for (unsigned int ndx = 0; ndx < NUM_CORNERS; ++ndx) {
      corners[ndx] = glm::vec3(transform * unitCorners[ndx]);
   }

   this->_corners = corners;
}

void BoundingBox::generateFaceNormals() {
   auto corners = this->getCorners();

   FaceNormals normals {{
      Plane::fromPoints(corners[5], corners[7], corners[3]).normal, // 0:right: fbr ftr ntr
      Plane::fromPoints(corners[3], corners[7], corners[6]).normal, // 1:top:   ntr ftr ftl
      Plane::fromPoints(corners[6], corners[7], corners[5]).normal, // 2:far:   ftl ftr fbr
   }};

   this->_faceNormals = normals;
}

void BoundingBox::generateDiagonalDirections() {
   auto corners = this->getCorners();

   DiagonalDirections diagonals;
   for (unsigned int ndx = 0; ndx < NUM_DIAGONALS; ++ndx) {
      const glm::vec3& v1 = corners[ndx];
      const glm::vec3& v2 = corners[(NUM_CORNERS - 1) - ndx];
      diagonals[ndx] = glm::normalize(glm::vec3(v2 - v1));
   }

   this->_diagonalDirections = diagonals;
}

bool BoundingBox::intersectAsSpheres(BoundingBox& boundingBox) {
   float minimumDistance = this->getRadius() + boundingBox.getRadius();
   glm::vec3 centerDistance = this->getPosition() - boundingBox.getPosition();

   return approxGreaterThan(minimumDistance * minimumDistance,
    glm::dot(centerDistance, centerDistance));
}

bool BoundingBox::intersectAsBoxes(BoundingBox& boundingBox) {
   BoundingBox& a = *this;
   BoundingBox& b = boundingBox;

   bool parallelPairExists = false;

   glm::vec3 centerDistance = b.getPosition() - a.getPosition();

   glm::vec3 aSize = a.getSize() * 0.5f;
   glm::vec3 bSize = b.getSize() * 0.5f;

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

         int index = linearize_index(glm::ivec2(aNdx, bNdx), glm::ivec2(3, 3));

         coefMatrix[index] = c;
         absCoefMatrix[index] = absC;
         if (approxGreaterThan(absC, 1.0f)) {
            parallelPairExists = true;
         }
      }

      float aDotD = glm::dot(aNormals[aNdx], centerDistance);
      normalDots[aNdx] = aDotD;

      float interval = std::abs(aDotD);
      float aRadius = aSize[aNdx];
      float bRadius = 0.0f;
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         int index = linearize_index(glm::ivec2(aNdx, bNdx), glm::ivec2(3, 3));

         bRadius += bSize[bNdx] * absCoefMatrix[index];
      }

      if (approxGreaterThan(interval, aRadius + bRadius)) {
         return parallelPairExists;
      }
   }

   // Assess face normals from other BoundingBox.
   // We don't need to look for parallel axes anymore.
   for (int bNdx = 0; bNdx < 3; ++bNdx) {
      float interval = std::abs(glm::dot(bNormals[bNdx], centerDistance));
      float aRadius = 0.0f;
      for (int aNdx = 0; aNdx < 3; ++aNdx) {
         int index = linearize_index(glm::ivec2(aNdx, bNdx), glm::ivec2(3, 3));

         aRadius += aSize[aNdx] * absCoefMatrix[index];
      }
      float bRadius = bSize[bNdx];

      if (approxGreaterThan(interval, aRadius + bRadius)) {
         return parallelPairExists;
      }
   }

   if (parallelPairExists) {
      return true;
   }

   // Assess axes generated from face normal cross products.
   for (int aNdx = 0; aNdx < 3; ++aNdx) {
      for (int bNdx = 0; bNdx < 3; ++bNdx) {
         float interval = BoundingBox::centerDifferenceProjection(
          aNdx, bNdx, normalDots, coefMatrix);
         float aRadius = BoundingBox::thisIntersectionRadiusProjection(
          aNdx, bNdx, aSize, absCoefMatrix);
         float bRadius = BoundingBox::otherIntersectionRadiusProjection(
          aNdx, bNdx, bSize, absCoefMatrix);

         if (approxGreaterThan(interval, aRadius + bRadius)) {
            return false;
         }
      }
   }

   return true;
}

/* static */ float BoundingBox::centerDifferenceProjection(int aNdx, int bNdx,
 const glm::vec3& normalDots, CoefficientMatrix coefMatrix) {
   int aDotNdx = (aNdx + 2) % 3;
   int bDotNdx = (aNdx + 1) % 3;

   int aCoefIndex = linearize_index(glm::ivec2(bDotNdx, bNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = linearize_index(glm::ivec2(aDotNdx, bNdx),
    glm::ivec2(3, 3));

   float aCoef = coefMatrix[aCoefIndex];
   float bCoef = coefMatrix[bCoefIndex];

   return std::abs(normalDots[aDotNdx] * aCoef - normalDots[bDotNdx] * bCoef);
}

/* static */ float BoundingBox::thisIntersectionRadiusProjection(int aNdx, int bNdx,
 const glm::vec3& aSize, CoefficientMatrix absCoefMatrix) {
   int aSizeNdx = (aNdx + 1) % 3;
   int bSizeNdx = (aNdx + 2) % 3;

   int aCoefIndex = linearize_index(glm::ivec2(bSizeNdx, bNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = linearize_index(glm::ivec2(aSizeNdx, bNdx),
    glm::ivec2(3, 3));

   float aCoef = absCoefMatrix[aCoefIndex];
   float bCoef = absCoefMatrix[bCoefIndex];

   return aSize[aSizeNdx] * aCoef + aSize[bSizeNdx] * bCoef;
}

/* static */ float BoundingBox::otherIntersectionRadiusProjection(int aNdx, int bNdx,
 const glm::vec3& bSize, CoefficientMatrix absCoefMatrix) {
   int aSizeNdx = bNdx ? 0 : 1;
   int bSizeNdx = (bNdx == 2) ? 1 : 2;

   int aCoefIndex = linearize_index(glm::ivec2(aNdx, bSizeNdx),
    glm::ivec2(3, 3));
   int bCoefIndex = linearize_index(glm::ivec2(aNdx, aSizeNdx),
    glm::ivec2(3, 3));

   float aCoef = absCoefMatrix[aCoefIndex];
   float bCoef = absCoefMatrix[bCoefIndex];

   return bSize[aSizeNdx] * aCoef + bSize[aSizeNdx] * bCoef;
}
