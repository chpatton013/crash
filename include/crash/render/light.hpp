#pragma once

#include <glm/glm.hpp>
#include <crash/common/movable.hpp>

namespace crash {
namespace render {

class Light : public common::Movable {
public:
   Light(const Light& light);
   Light(const glm::vec3& position, const glm::vec4& diffuse,
    const glm::vec4& specular);
   virtual ~Light();

   /////////////////////////////////////////////////////////////////////////////
   // Movable interface.
   /////////////////////////////////////////////////////////////////////////////

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

   /////////////////////////////////////////////////////////////////////////////
   // Light.
   /////////////////////////////////////////////////////////////////////////////

   const glm::vec4& getDiffuse() const;
   const glm::vec4& getSpecular() const;

   void setDiffuse(const glm::vec4& diffuse);
   void setSpecular(const glm::vec4& specular);

private:
   glm::vec3 _position;
   glm::vec4 _diffuse;
   glm::vec4 _specular;
   glm::mat4 _transform;
};

} // namespace render
} // namespace crash
