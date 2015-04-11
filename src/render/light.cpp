#include <glm/gtc/matrix_transform.hpp>
#include <crash/render/light.hpp>

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
// Transformable interface.
////////////////////////////////////////////////////////////////////////////////

const glm::vec3& Light::getPosition() const {
   return this->_position;
}

const glm::vec4& Light::getOrientation() const {
   return Light::__orientation;
}

const glm::vec3& Light::getSize() const {
   return Light::__size;
}

void Light::setPosition(const glm::vec3& position) {
   this->_position = position;
}

void Light::setOrientation(const glm::vec4&) {
}

void Light::setSize(const glm::vec3&) {
}

const glm::mat4& Light::getTransform() {
   this->_transform = glm::translate(glm::mat4(), this->_position);
   return this->_transform;
}

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

/* static */ glm::vec4 Light::__orientation(1.0f, 0.0f, 0.0f, 0.0f);
/* static */ glm::vec3 Light::__size(1.0f);
