#pragma once

#include <glm/glm.hpp>
#include <crash/common/movable.hpp>

namespace crash {
namespace common {

class Transformer : public Movable {
public:
   Transformer(const Transformer& transformer);
   Transformer(glm::vec3 position, glm::quat orientation, glm::vec3 size,
    glm::vec3 translationalVelocity, glm::quat rotationalVelocity,
    glm::vec3 scaleVelocity);
   virtual ~Transformer();

   glm::vec3 getPosition() const;
   glm::quat getOrientation() const;
   glm::vec3 getSize() const;
   glm::vec3 getTranslationalVelocity() const;
   glm::quat getRotationalVelocity() const;
   glm::vec3 getScaleVelocity() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::quat& orientation);
   void setSize(const glm::vec3& size);
   void setTranslationalVelocity(const glm::vec3& translationalVelocity);
   void setRotationalVelocity(const glm::quat& rotationalVelocity);
   void setScaleVelocity(const glm::vec3& scaleVelocity);

private:
   glm::vec3 _position;
   glm::quat _orientation;
   glm::vec3 _size;
   glm::vec3 _translationalVelocity;
   glm::quat _rotationalVelocity;
   glm::vec3 _scaleVelocity;
};

} // namespace common
} // namespace crash
