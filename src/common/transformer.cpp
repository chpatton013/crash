#include <crash/common/transformer.hpp>

using namespace crash::common;

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
