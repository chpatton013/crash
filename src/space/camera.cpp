#include <glm/gtc/matrix_transform.hpp>
#include <crash/space/camera.hpp>

using namespace crash::math;
using namespace crash::space;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Camera::Camera(const Camera& camera) :
   Camera(camera._transformer, camera._fieldOfView, camera._aspectRatio,
    camera._nearPlane, camera._farPlane)
{}

Camera::Camera(const math::Transformer& transformer, float fieldOfView,
 float aspectRatio, float nearPlane, float farPlane) :
   _transformer(transformer), _fieldOfView(fieldOfView),
    _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const glm::vec3& Camera::getPosition() const {
   return this->_transformer.getPosition();
}

const glm::vec4& Camera::getOrientation() const {
   return this->_transformer.getOrientation();
}

float Camera::getFieldOfView() const {
   return this->_fieldOfView;
}

float Camera::getAspectRatio() const {
   return this->_aspectRatio;
}

float Camera::getNearPlane() const {
   return this->_nearPlane;
}

float Camera::getFarPlane() const {
   return this->_farPlane;
}

const Transformer& Camera::getTransformer() const {
   return this->_transformer;
}

void Camera::setPosition(const glm::vec3& position) {
   this->_transformer.setPosition(position);
   this->invalidate();
}

void Camera::setOrientation(const glm::vec4& orientation) {
   this->_transformer.setOrientation(orientation);
   this->invalidate();
}

void Camera::setFieldOfView(float fieldOfView) {
   this->_fieldOfView = fieldOfView;
   this->invalidate();
}

void Camera::setAspectRatio(float aspectRatio) {
   this->_aspectRatio = aspectRatio;
   this->invalidate();
}

void Camera::setNearPlane(float nearPlane) {
   this->_nearPlane = nearPlane;
   this->invalidate();
}

void Camera::setFarPlane(float farPlane) {
   this->_farPlane = farPlane;
   this->invalidate();
}

void Camera::setTransformer(const Transformer& transformer) {
   this->_transformer = transformer;
   this->invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// Memoization
////////////////////////////////////////////////////////////////////////////////

void Camera::invalidate() {
   this->_transformer.invalidate();

   this->_perspective = boost::none;
   this->_viewFrustum = boost::none;
}

const glm::mat4& Camera::getTransform() {
   return this->_transformer.getSizeInvariantTransform();
}

const glm::mat4& Camera::getPerspective() {
   if (!this->_perspective) {
      this->calculatePerspective();
   }

   return this->_perspective.get();
}

const ViewFrustum& Camera::getViewFrustum() {
   if (!this->_viewFrustum) {
      this->calculateViewFrustum();
   }

   return this->_viewFrustum.get();
}

void Camera::calculatePerspective() {
   this->_perspective = glm::perspective(this->_fieldOfView,
    this->_aspectRatio, this->_nearPlane, this->_farPlane);
}

void Camera::calculateViewFrustum() {
   this->_viewFrustum = ViewFrustum::fromValues(this->_fieldOfView,
    this->_aspectRatio, this->_nearPlane, this->_farPlane,
    this->getTransform());
}
