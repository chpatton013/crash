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
   GLint getVariableHandle(const std::string& name) const;

   void setUniformVariable1f(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariable2f(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariable3f(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariable4f(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariable1i(const std::string& name, const GLint* array,
    GLsizei length) const;
   void setUniformVariable2i(const std::string& name, const GLint* array,
    GLsizei length) const;
   void setUniformVariable3i(const std::string& name, const GLint* array,
    GLsizei length) const;
   void setUniformVariable4i(const std::string& name, const GLint* array,
    GLsizei length) const;
   void setUniformVariable1ui(const std::string& name, const GLuint* array,
    GLsizei length) const;
   void setUniformVariable2ui(const std::string& name, const GLuint* array,
    GLsizei length) const;
   void setUniformVariable3ui(const std::string& name, const GLuint* array,
    GLsizei length) const;
   void setUniformVariable4ui(const std::string& name, const GLuint* array,
    GLsizei length) const;
   void setUniformVariableMatrix2(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariableMatrix3(const std::string& name, const GLfloat* array,
    GLsizei length) const;
   void setUniformVariableMatrix4(const std::string& name, const GLfloat* array,
    GLsizei length) const;

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
