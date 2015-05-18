#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <crash/common/movable.hpp>
#include <crash/common/symbols.hpp>
#include <crash/common/util.hpp>

using namespace crash::common;

void Movable::translate(const glm::vec3& translation) {
   this->setPosition(this->getPosition() + translation);
}

void Movable::rotate(const glm::quat& rotation) {
   this->setOrientation(rotation * this->getOrientation());
}

void Movable::scale(const glm::vec3& scale) {
   this->setSize(this->getSize() + this->getSize() * scale);
}

void Movable::translate(float delta_t) {
   this->translate(this->getTranslationalDelta(delta_t));
}

void Movable::rotate(float delta_t) {
   this->rotate(this->getRotationalDelta(delta_t));
}

void Movable::scale(float delta_t) {
   this->scale(this->getScaleDelta(delta_t));
}

void Movable::move(float delta_t) {
   this->translate(delta_t);
   this->rotate(delta_t);
   this->scale(delta_t);
}

glm::vec3 Movable::getUp() {
   return glm::normalize(glm::vec3(glm::mat4_cast(this->getOrientation()) *
    glm::vec4(UP, 1.0f)));
}

glm::vec3 Movable::getForward() {
   return glm::normalize(glm::vec3(glm::mat4_cast(this->getOrientation()) *
    glm::vec4(FORWARD, 1.0f)));
}

glm::vec3 Movable::getTranslationalDelta(float delta_t) const {
   return this->getTranslationalVelocity() * delta_t;
}

glm::quat Movable::getRotationalDelta(float delta_t) const {
   return glm::slerp(glm::quat(), this->getRotationalVelocity(), delta_t);
}

glm::vec3 Movable::getScaleDelta(float delta_t) const {
   return this->getScaleVelocity() * delta_t;
}

glm::mat4 Movable::getTransform() const {
   auto translation = glm::translate(this->getPosition());
   auto rotation = glm::mat4_cast(this->getOrientation());
   auto scale = glm::scale(this->getSize());
   return translation * rotation * scale;
}

glm::mat4 Movable::getTransform(float delta_t) const {
   auto translation = glm::translate(this->getPosition() +
    this->getTranslationalDelta(delta_t));
   auto rotation = glm::mat4_cast(this->getRotationalDelta(delta_t) *
    this->getOrientation());
   auto scale = glm::scale(this->getSize() +
    this->getSize() * this->getScaleDelta(delta_t));
   return translation * rotation * scale;
}

Transformer::Transformer(const Transformer& transformer) :
   Transformer(transformer._position, transformer._orientation,
    transformer._size, transformer._translationalVelocity,
    transformer._rotationalVelocity, transformer._scaleVelocity)
{}

Transformer::Transformer(glm::vec3 position, glm::quat orientation,
 glm::vec3 size, glm::vec3 translationalVelocity,
 glm::quat rotationalVelocity, glm::vec3 scaleVelocity) :
   _position(position), _orientation(orientation), _size(size),
    _translationalVelocity(translationalVelocity),
    _rotationalVelocity(rotationalVelocity),
    _scaleVelocity(scaleVelocity)
{}

/* virtual */ Transformer::~Transformer() {}

glm::vec3 Transformer::getPosition() const {
   return this->_position;
}

glm::quat Transformer::getOrientation() const {
   return this->_orientation;
}

glm::vec3 Transformer::getSize() const {
   return this->_size;
}

glm::vec3 Transformer::getTranslationalVelocity() const {
   return this->_translationalVelocity;
}

glm::quat Transformer::getRotationalVelocity() const {
   return this->_rotationalVelocity;
}

glm::vec3 Transformer::getScaleVelocity() const {
   return this->_scaleVelocity;
}

void Transformer::setPosition(const glm::vec3& position) {
   this->_position = position;
}

void Transformer::setOrientation(const glm::quat& orientation) {
   this->_orientation = orientation;
}

void Transformer::setSize(const glm::vec3& size) {
   this->_size = size;
}

void Transformer::setTranslationalVelocity(const glm::vec3& translationalVelocity) {
   this->_translationalVelocity = translationalVelocity;
}

void Transformer::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->_rotationalVelocity = rotationalVelocity;
}

void Transformer::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_scaleVelocity = scaleVelocity;
}
