#include <crash/engine/camera.hpp>
#include <crash/render/view_frustum.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace crash::engine;
using namespace crash::render;
using namespace crash::space;

Camera::Camera(const Camera& camera) :
   _boundingBox(camera._boundingBox),
    _verticalFieldOfView(camera._verticalFieldOfView),
    _aspectRatio(camera._aspectRatio),
    _nearPlane(camera._nearPlane),
    _farPlane(camera._farPlane),
    _orthographic(boost::none),
    _perspective(boost::none),
    _lookAt(boost::none),
    _viewFrustum(boost::none)
{}

Camera::Camera(const space::BoundingBox& boundingBox,
 float verticalFieldOfView, float aspectRatio,
 float nearPlane, float farPlane) :
   _boundingBox(boundingBox),
    _verticalFieldOfView(verticalFieldOfView),
    _aspectRatio(aspectRatio),
    _nearPlane(nearPlane),
    _farPlane(farPlane),
    _orthographic(boost::none),
    _perspective(boost::none),
    _lookAt(boost::none),
    _viewFrustum(boost::none)
{}

/* virtual */ Camera::~Camera() {}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 Camera::getPosition() const {
   return this->_boundingBox.getPosition();
}

glm::quat Camera::getOrientation() const {
   return this->_boundingBox.getOrientation();
}

glm::vec3 Camera::getSize() const {
   return this->_boundingBox.getSize();
}

glm::vec3 Camera::getTranslationalVelocity() const {
   return this->_boundingBox.getTranslationalVelocity();
}

glm::quat Camera::getRotationalVelocity() const {
   return this->_boundingBox.getRotationalVelocity();
}

glm::vec3 Camera::getScaleVelocity() const {
   return this->_boundingBox.getScaleVelocity();
}

void Camera::setPosition(const glm::vec3& position) {
   this->_boundingBox.setPosition(position);
   this->invalidate();
}

void Camera::setOrientation(const glm::quat& orientation) {
   this->_boundingBox.setOrientation(orientation);
   this->invalidate();
}

void Camera::setSize(const glm::vec3& size) {
   this->_boundingBox.setSize(size);
   this->invalidate();
}

void Camera::setTranslationalVelocity(const glm::vec3& translationalVelocity) {
   this->_boundingBox.setTranslationalVelocity(translationalVelocity);
   this->invalidate();
}

void Camera::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->_boundingBox.setRotationalVelocity(rotationalVelocity);
   this->invalidate();
}

void Camera::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_boundingBox.setScaleVelocity(scaleVelocity);
   this->invalidate();
}

void Camera::translate(const glm::vec3& translation) {
   this->_boundingBox.translate(translation);
   this->invalidate();
}

void Camera::rotate(const glm::quat& rotation) {
   this->_boundingBox.rotate(rotation);
   this->invalidate();
}

void Camera::scale(const glm::vec3& scale) {
   this->_boundingBox.scale(scale);
   this->invalidate();
}

void Camera::translate(float delta_t) {
   this->_boundingBox.translate(delta_t);
   this->invalidate();
}

void Camera::rotate(float delta_t) {
   this->_boundingBox.rotate(delta_t);
   this->invalidate();
}

void Camera::scale(float delta_t) {
   this->_boundingBox.scale(delta_t);
   this->invalidate();
}

void Camera::move(float delta_t) {
   this->_boundingBox.move(delta_t);
   this->invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// Boundable interface.
/////////////////////////////////////////////////////////////////////////////

BoundingBox* Camera::getBoundingBox() {
   return &this->_boundingBox;
}

/////////////////////////////////////////////////////////////////////////////
// Data access.
/////////////////////////////////////////////////////////////////////////////

float Camera::getVerticalFieldOfView() const {
   return this->_verticalFieldOfView;
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

void Camera::setVerticalFieldOfView(float verticalFieldOfView) {
   this->_verticalFieldOfView = verticalFieldOfView;
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

/////////////////////////////////////////////////////////////////////////////
// Memoization
/////////////////////////////////////////////////////////////////////////////

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
   float fovY = this->getVerticalFieldOfView();
   float fovX = fovY * this->getAspectRatio();
   float tanX = tan(fovX * 0.5f);
   float tanY = tan(fovY * 0.5f);
   float x = tanX * this->getFarPlane();
   float y = tanY * this->getFarPlane();
   this->_orthographic = glm::ortho(-x, x, -y, y,
    this->getNearPlane(), this->getFarPlane());
}

void Camera::calculatePerspective() {
   this->_perspective = glm::perspective(this->getVerticalFieldOfView(),
    this->getAspectRatio(), this->getNearPlane(), this->getFarPlane());
}

void Camera::calculateLookAt() {
   this->_lookAt = glm::lookAt(this->getPosition(),
    this->getPosition() + this->getForward(), this->getUp());
}

void Camera::calculateViewFrustum() {
   this->_viewFrustum = ViewFrustum::fromValues(
    this->getVerticalFieldOfView(), this->getAspectRatio(),
    this->getNearPlane(), this->getFarPlane(),
    this->getLookAt());
}
