#include <glm/gtc/matrix_transform.hpp>
#include <crash/math/math.hpp>
#include <crash/space/camera.hpp>
#include <crash/util/type.hpp>

using namespace crash::space;

Camera::Camera(const glm::vec3& position, const glm::vec4& orientation,
 float fieldOfView, float aspectRatio, float nearPlane, float farPlane) :
   _position(position), _orientation(orientation), _fieldOfView(fieldOfView),
    _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{}

///////////////////////////////////////////////////////////////////////////////
// Data getters
///////////////////////////////////////////////////////////////////////////////

const glm::vec3& Camera::position() const { return this->_position; }
const glm::vec4& Camera::orientation() const { return this->_orientation; }
float Camera::fieldOfView() const { return this->_fieldOfView; }
float Camera::aspectRatio() const { return this->_aspectRatio; }
float Camera::nearPlane() const { return this->_nearPlane; }
float Camera::farPlane() const { return this->_farPlane; }

///////////////////////////////////////////////////////////////////////////////
// Data setters
///////////////////////////////////////////////////////////////////////////////

void Camera::position(const glm::vec3& position) {
   this->invalidate();
   this->_position = position;
}

void Camera::orientation(const glm::vec4& orientation) {
   this->invalidate();
   this->_orientation = orientation;
}

void Camera::fieldOfView(float fieldOfView) {
   this->invalidate();
   this->_fieldOfView = fieldOfView;
}

void Camera::aspectRatio(float aspectRatio) {
   this->invalidate();
   this->_aspectRatio = aspectRatio;
}

void Camera::nearPlane(float nearPlane) {
   this->invalidate();
   this->_nearPlane = nearPlane;
}

void Camera::farPlane(float farPlane) {
   this->invalidate();
   this->_farPlane = farPlane;
}

///////////////////////////////////////////////////////////////////////////////
// Memoized getters
///////////////////////////////////////////////////////////////////////////////

const glm::mat4& Camera::getTransform() {
   if (!this->_transform) {
      this->generateTransform();
   }

   return this->_transform.get();
}

const glm::mat4& Camera::getPerspective() {
   if (!this->_perspective) {
      this->generatePerspective();
   }

   return this->_perspective.get();
}

const ViewFrustum& Camera::getViewFrustum() {
   if (!this->_viewFrustum) {
      this->generateViewFrustum();
   }

   return this->_viewFrustum.get();
}

///////////////////////////////////////////////////////////////////////////////
// Other methods
///////////////////////////////////////////////////////////////////////////////

bool Camera::isPointVisible(const glm::vec3& point) {
   for (CAUTO plane : this->getViewFrustum().getPlanes()) {
      if (plane.distance(point) < 0) {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
// Memoization
///////////////////////////////////////////////////////////////////////////////

void Camera::invalidate() {
   this->_transform = boost::optional< glm::mat4 >();
   this->_perspective = boost::optional< glm::mat4 >();
   this->_viewFrustum = boost::optional< ViewFrustum >();
}

void Camera::generateTransform() {
   glm::mat4 transform = math::transform(this->_position, this->_orientation);

   this->_transform = boost::optional< glm::mat4 >(transform);
}

void Camera::generatePerspective() {
   glm::mat4 perspective = glm::perspective(this->_fieldOfView,
    this->_aspectRatio, this->_nearPlane, this->_farPlane);

   this->_perspective = boost::optional< glm::mat4 >(perspective);
}

void Camera::generateViewFrustum() {
   ViewFrustum viewFrustum = ViewFrustum::fromValues(this->_fieldOfView,
    this->_aspectRatio, this->_nearPlane, this->_farPlane, this->getTransform());

   this->_viewFrustum = boost::optional< ViewFrustum >(viewFrustum);
}
