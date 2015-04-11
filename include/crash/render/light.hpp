#pragma once

#include <glm/glm.hpp>
#include <crash/math/transformable.hpp>

namespace crash {
namespace render {

class Light : public math::Transformable {
public:
   Light(const Light& light);
   Light(const glm::vec3& position, const glm::vec4& diffuse,
    const glm::vec4& specular);
   virtual ~Light();

   /////////////////////////////////////////////////////////////////////////////
   // Transformable interface.
   /////////////////////////////////////////////////////////////////////////////

   const glm::vec3& getPosition() const;
   const glm::vec4& getOrientation() const;
   const glm::vec3& getSize() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::vec4& orientation);
   void setSize(const glm::vec3& size);

   const glm::mat4& getTransform();

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

   static glm::vec4 __orientation;
   static glm::vec3 __size;
};

} // namespace render
} // namespace crash
