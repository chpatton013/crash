#include <crash/render/light.hpp>
#include <crash/common/symbols.hpp>

using namespace crash::common;
using namespace crash::render;

Light::Light(const Light& light) :
   _position(light._position), _diffuse(light._diffuse),
    _specular(light._specular)
{}

Light::Light(const glm::vec3& position, const glm::vec4& diffuse,
 const glm::vec4& specular) :
   _position(position), _diffuse(diffuse), _specular(specular)
{}

/* virtual */ Light::~Light() {}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 Light::getPosition() const {
   return this->_position;
}

glm::quat Light::getOrientation() const {
   return NO_ROTATION;
}

glm::vec3 Light::getSize() const {
   return UNIT_SIZE;
}

glm::vec3 Light::getTranslationalVelocity() const {
   return glm::vec3();
}

glm::quat Light::getRotationalVelocity() const {
   return NO_ROTATION;
}

glm::vec3 Light::getScaleVelocity() const {
   return glm::vec3();
}

void Light::setPosition(const glm::vec3& position) {
   this->_position = position;
}

void Light::setOrientation(const glm::quat&) {}

void Light::setSize(const glm::vec3&) {}

void Light::setTranslationalVelocity(const glm::vec3&) {}

void Light::setRotationalVelocity(const glm::quat&) {}

void Light::setScaleVelocity(const glm::vec3&) {}

////////////////////////////////////////////////////////////////////////////////
// Light.
////////////////////////////////////////////////////////////////////////////////

const glm::vec4& Light::getDiffuse() const {
   return this->_diffuse;
}

const glm::vec4& Light::getSpecular() const {
   return this->_specular;
}

void Light::setDiffuse(const glm::vec4& diffuse) {
   this->_diffuse = diffuse;
}

void Light::setSpecular(const glm::vec4& specular) {
   this->_specular = specular;
}
