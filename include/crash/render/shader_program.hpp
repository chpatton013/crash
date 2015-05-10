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

struct AttributeVariable {
   AttributeVariable(const std::string& position, const std::string& normal,
    const std::string& tangent, const std::string& bitangent,
    const std::string& texture_coordinates,
    const std::string& bone_ids, const std::string& bone_weights);

   std::string position;
   std::string normal;
   std::string tangent;
   std::string bitangent;
   std::string texture_coordinates;
   std::string bone_ids;
   std::string bone_weights;
};

struct UniformVariable {
   UniformVariable(
    const std::string& model_transform,
    const std::string& view_transform,
    const std::string& perspective_transform,
    const std::string& bones,
    const std::string& camera_position,
    const std::string& light_position,
    const std::string& light_diffuse,
    const std::string& light_specular,
    const std::string& ambient_color,
    const std::string& diffuse_color,
    const std::string& specular_color,
    const std::string& shininess_value,
    const std::string& has_displacement_texture,
    const std::string& has_normal_texture,
    const std::string& has_ambient_texture,
    const std::string& has_diffuse_texture,
    const std::string& has_specular_texture,
    const std::string& has_shininess_texture,
    const std::string& displacement_texture,
    const std::string& normal_texture,
    const std::string& ambient_texture,
    const std::string& diffuse_texture,
    const std::string& specular_texture,
    const std::string& shininess_texture);

   std::string model_transform;
   std::string view_transform;
   std::string perspective_transform;
   std::string bones;
   std::string camera_position;
   std::string light_position;
   std::string light_diffuse;
   std::string light_specular;
   std::string ambient_color;
   std::string diffuse_color;
   std::string specular_color;
   std::string shininess_value;
   std::string has_displacement_texture;
   std::string has_normal_texture;
   std::string has_ambient_texture;
   std::string has_diffuse_texture;
   std::string has_specular_texture;
   std::string has_shininess_texture;
   std::string displacement_texture;
   std::string normal_texture;
   std::string ambient_texture;
   std::string diffuse_texture;
   std::string specular_texture;
   std::string shininess_texture;
};

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
