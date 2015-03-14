#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <crash/math/transformer.hpp>

using namespace crash::math;

Transformer::Transformer(const Transformer& transformer) :
   Transformer(transformer._position, transformer._orientation,
    transformer._size)
{}

Transformer::Transformer(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size) :
   _position(position), _orientation(orientation), _size(size)
{}

const glm::vec3& Transformer::getPosition() const {
   return this->_position;
}

const glm::vec4& Transformer::getOrientation() const {
   return this->_orientation;
}

const glm::vec3& Transformer::getSize() const {
   return this->_size;
}

void Transformer::setPosition(const glm::vec3& position) {
   this->_position = position;
}

void Transformer::setOrientation(const glm::vec4& orientation) {
   this->_orientation = orientation;
}

void Transformer::setSize(const glm::vec3& size) {
   this->_size = size;
}

void Transformer::invalidate() {
   this->_transform = boost::none;
   this->_positionInvariantTransform = boost::none;
   this->_orientationInvariantTransform = boost::none;
   this->_sizeInvariantTransform = boost::none;
}

glm::mat4 Transformer::getTranslateTransform() {
   return glm::translate(this->getPosition());
}

glm::mat4 Transformer::getRotateTransform() {
   return glm::rotate(this->getOrientation().w,
    glm::vec3(this->getOrientation()));
}

glm::mat4 Transformer::getScaleTransform() {
   return glm::scale(this->getSize());
}

const glm::mat4& Transformer::getPositionInvariantTransform() {
   if (!this->_positionInvariantTransform) {
      this->calculatePositionInvariantTransform();
   }

   return this->_positionInvariantTransform.get();
}

const glm::mat4& Transformer::getOrientationInvariantTransform() {
   if (!this->_orientationInvariantTransform) {
      this->calculateOrientationInvariantTransform();
   }

   return this->_orientationInvariantTransform.get();
}

const glm::mat4& Transformer::getSizeInvariantTransform() {
   if (!this->_sizeInvariantTransform) {
      this->calculateSizeInvariantTransform();
   }

   return this->_sizeInvariantTransform.get();
}

const glm::mat4& Transformer::getTransform() {
   if (!this->_transform) {
      this->calculateTransform();
   }

   return this->_transform.get();
}

void Transformer::applyTranslation(const glm::vec3& translation) {
   this->setPosition(this->getPosition() + translation);
}

void Transformer::applyRotation(const glm::vec4& rotation) {
   this->setOrientation(glm::rotate(this->getOrientation(), rotation.w,
    glm::vec3(rotation)));
}

void Transformer::applyScale(const glm::vec3& scale) {
   this->setSize(this->getSize() * scale);
}

void Transformer::calculatePositionInvariantTransform() {
   this->_positionInvariantTransform = this->getRotateTransform() *
    this->getScaleTransform();
}

void Transformer::calculateOrientationInvariantTransform() {
   this->_orientationInvariantTransform = this->getTranslateTransform() *
    this->getScaleTransform();
}

void Transformer::calculateSizeInvariantTransform() {
   this->_sizeInvariantTransform = this->getTranslateTransform() *
    this->getRotateTransform();
}

void Transformer::calculateTransform() {
   this->_transform = this->getTranslateTransform() *
    this->getRotateTransform() * this->getScaleTransform();
}
