#pragma once

#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace crash {
namespace common {

class Movable {
public:
   virtual ~Movable() {};

   virtual glm::vec3 getPosition() const = 0;
   virtual glm::quat getOrientation() const = 0;
   virtual glm::vec3 getSize() const = 0;
   virtual glm::vec3 getTranslationalVelocity() const = 0;
   virtual glm::quat getRotationalVelocity() const = 0;
   virtual glm::vec3 getScaleVelocity() const = 0;

   virtual void setPosition(const glm::vec3& position) = 0;
   virtual void setOrientation(const glm::quat& orientation) = 0;
   virtual void setSize(const glm::vec3& size) = 0;
   virtual void setTranslationalVelocity(
    const glm::vec3& translationalVelocity) = 0;
   virtual void setRotationalVelocity(const glm::quat& rotationalVelocity) = 0;
   virtual void setScaleVelocity(const glm::vec3& scaleVelocity) = 0;

   void translate(const glm::vec3& translation);
   void rotate(const glm::quat& rotation);
   void scale(const glm::vec3& scale);

   void translate(float delta_t);
   void rotate(float delta_t);
   void scale(float delta_t);

   void move(float delta_t);

   glm::vec3 getUp();
   glm::vec3 getForward();

   glm::vec3 getTranslationalDelta(float delta_t) const;
   glm::quat getRotationalDelta(float delta_t) const;
   glm::vec3 getScaleDelta(float delta_t) const;

   glm::mat4 getTransform() const;
   glm::mat4 getTransform(float delta_t) const;
};

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
