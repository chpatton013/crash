#include <glm/glm.hpp>
#include <crash/engine/actor.hpp>

using namespace crash::engine;
using namespace crash::render;
using namespace crash::space;

Actor::Actor(const Actor& actor) :
   _boundingBox(actor._boundingBox), _meshInstance(actor._meshInstance)
{}

Actor::Actor(const BoundingBox& _boundingBox,
 const MeshInstance& _meshInstance) :
   _boundingBox(_boundingBox), _meshInstance(_meshInstance)
{}

/* virtual */ Actor::~Actor() {}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 Actor::getPosition() const {
   return this->_boundingBox.getPosition();
}

glm::quat Actor::getOrientation() const {
   return this->_boundingBox.getOrientation();
}

glm::vec3 Actor::getSize() const {
   return this->_boundingBox.getSize();
}

glm::vec3 Actor::getTranslationalVelocity() const {
   return this->_boundingBox.getTranslationalVelocity();
}

glm::quat Actor::getRotationalVelocity() const {
   return this->_boundingBox.getRotationalVelocity();
}

glm::vec3 Actor::getScaleVelocity() const {
   return this->_boundingBox.getScaleVelocity();
}

void Actor::setPosition(const glm::vec3& position) {
   this->_boundingBox.setPosition(position);
}

void Actor::setOrientation(const glm::quat& orientation) {
   this->_boundingBox.setOrientation(orientation);
}

void Actor::setSize(const glm::vec3& size) {
   this->_boundingBox.setSize(size);
}

void Actor::setTranslationalVelocity(const glm::vec3& translationalVelocity) {
   this->_boundingBox.setTranslationalVelocity(translationalVelocity);
}

void Actor::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->_boundingBox.setRotationalVelocity(rotationalVelocity);
}

void Actor::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->_boundingBox.setScaleVelocity(scaleVelocity);
}

////////////////////////////////////////////////////////////////////////////////
// Boundable interface.
////////////////////////////////////////////////////////////////////////////////

BoundingBox* Actor::getBoundingBox() {
   return &this->_boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
// Renderable interface.
////////////////////////////////////////////////////////////////////////////////

MeshInstance* Actor::getMeshInstance() {
   return &this->_meshInstance;
}

void Actor::render(float delta_t) {
   return this->getMeshInstance()->render(this->getTransform(), delta_t);
}
