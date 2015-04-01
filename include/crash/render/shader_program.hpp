#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <crash/render/shader.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

namespace crash {
namespace render {

class ShaderProgram {
public:
   typedef std::vector< std::shared_ptr< Shader > > Shaders;

   ShaderProgram(const Shaders& shaders);
   virtual ~ShaderProgram();

   const Shaders& getShaders() const;
   GLuint getHandle() const;

   void link() const;
   void use() const;
   void createUniformVariable(const std::string& name);
   void setUniformVariable(const std::string& name, GLboolean value) const;
   void setUniformVariable(const std::string& name, GLint value) const;
   void setUniformVariable(const std::string& name, GLuint value) const;
   void setUniformVariable(const std::string& name, GLfloat value) const;
   void setUniformVariable(const std::string& name,
    const glm::vec3& value) const;
   void setUniformVariable(const std::string& name,
    const glm::vec4& value) const;
   void setUniformVariable(const std::string& name,
    const glm::mat4& value) const;

   struct LinkFailure {
      LinkFailure(const std::string& error);
      std::string error;
   };

   struct VariableAllocationFailure {
      VariableAllocationFailure(const std::string& error);
      std::string error;
   };

   struct VariableReferenceFailure {
      VariableReferenceFailure(const std::string& error);
      std::string error;
   };

private:
   Shaders _shaders;
   GLuint _handle;
   std::map< std::string, GLint > _variables;
};

} // namespace render
} // namespace crash
