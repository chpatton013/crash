#include <crash/math/math.hpp>
#include <crash/math/transformer.hpp>

using namespace crash::math;

Transformer::Transformer(const Transformer& transformer) :
   Transformer(transformer._position, transformer._orientation, transformer._size)
{}

Transformer::Transformer(const glm::vec3& position,
 const glm::vec4& orientation, const glm::vec3& size) :
   _position(position), _orientation(orientation), _size(size)
{}

const glm::vec3& Transformer::position() const {
   return this->_position;
}

const glm::vec4& Transformer::orientation() const {
   return this->_orientation;
}

const glm::vec3& Transformer::size() const {
   return this->_size;
}

void Transformer::position(const glm::vec3& position) {
   this->invalidate();
   this->_position = position;
}

void Transformer::orientation(const glm::vec4& orientation) {
   this->invalidate();
   this->_orientation = orientation;
}

void Transformer::size(const glm::vec3& size) {
   this->invalidate();
   this->_size = size;
}

void Transformer::invalidate() {
   this->_transform = boost::none;
}

const glm::mat4& Transformer::getTransform() {
   if (!this->_transform) {
      this->generateTransform();
   }

   return this->_transform.get();
}

void Transformer::generateTransform() {
   this->_transform = math::transform(this->_position, this->_orientation,
    this->_size);
}
