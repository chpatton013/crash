#pragma once

#include <glm/glm.hpp>
#include <crash/math/transformable.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

namespace crash {
namespace render {

class Light : public math::Transformable {
public:
   struct ShaderLight {
      ShaderLight(const glm::vec3& position, const glm::vec4& diffuse,
       const glm::vec4& specular);

      glm::vec3 position;
      glm::vec4 diffuse;
      glm::vec4 specular;
   };

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

   ShaderLight toShaderLight();

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
