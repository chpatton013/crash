#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <crash/space/camera.hpp>

using namespace crash::math;
using namespace crash::space;

////////////////////////////////////////////////////////////////////////////////
// Constructors.
////////////////////////////////////////////////////////////////////////////////

Camera::Camera(const Camera& camera) :
   _position(camera._position), _forward(camera._forward), _up(camera._up),
    _fieldOfView(camera._fieldOfView), _aspectRatio(camera._aspectRatio),
    _nearPlane(camera._nearPlane), _farPlane(camera._farPlane)
{}

Camera::Camera(const glm::vec3& position, const glm::vec3& forward,
 const glm::vec3& up, float fieldOfView, float aspectRatio,
 float nearPlane, float farPlane) :
   _position(position), _forward(glm::normalize(forward)),
    _up(glm::normalize(up)),
    _fieldOfView(fieldOfView), _aspectRatio(aspectRatio),
    _nearPlane(nearPlane), _farPlane(farPlane)
{}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const glm::vec3& Camera::getPosition() const {
   return this->_position;
}

const glm::vec3& Camera::getForward() const {
   return this->_forward;
}

const glm::vec3& Camera::getUp() const {
   return this->_up;
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

void Camera::setPosition(const glm::vec3& position) {
   this->_position = position;
   this->invalidate();
}

void Camera::setForward(const glm::vec3& forward) {
   this->_forward = glm::normalize(forward);
   this->invalidate();
}

void Camera::setUp(const glm::vec3& up) {
   this->_up = glm::normalize(up);
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

////////////////////////////////////////////////////////////////////////////////
// Memoization
////////////////////////////////////////////////////////////////////////////////

void Camera::invalidate() {
   this->_orthographic = boost::none;
   this->_perspective = boost::none;
   this->_lookAt = boost::none;
   this->_viewFrustum = boost::none;
}

const glm::mat4& Camera::getOrthographic() {
   if (!this->_orthographic) {
      this->calculateOrthographic();
   }

   return this->_orthographic.get();
}

const glm::mat4& Camera::getPerspective() {
   if (!this->_perspective) {
      this->calculatePerspective();
   }

   return this->_perspective.get();
}

const glm::mat4& Camera::getLookAt() {
   if (!this->_lookAt) {
      this->calculateLookAt();
   }

   return this->_lookAt.get();
}

const ViewFrustum& Camera::getViewFrustum() {
   if (!this->_viewFrustum) {
      this->calculateViewFrustum();
   }

   return this->_viewFrustum.get();
}

void Camera::calculateOrthographic() {
   float fovY = this->getFieldOfView();
   float fovX = fovY * this->getAspectRatio();
   float tanX = tan(fovX * 0.5f);
   float tanY = tan(fovY * 0.5f);
   float x = tanX * this->_farPlane;
   float y = tanY * this->_farPlane;
   this->_orthographic = glm::ortho(-x, x, -y, y,
    this->_nearPlane, this->_farPlane);
}

void Camera::calculatePerspective() {
   this->_perspective = glm::perspective(this->_fieldOfView,
    this->_aspectRatio, this->_nearPlane, this->_farPlane);
}

void Camera::calculateLookAt() {
   this->_lookAt = glm::lookAt(this->_position,
    this->_position + this->_forward, this->_up);
}

void Camera::calculateViewFrustum() {
   this->_viewFrustum = ViewFrustum::fromValues(
    this->getFieldOfView(), this->getAspectRatio(),
    this->getNearPlane(), this->getFarPlane(),
    this->getLookAt());
}
