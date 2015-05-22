#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>

using namespace crash::render;

AttributeVariable::AttributeVariable(
 const std::string& position, const std::string& normal,
 const std::string& tangent, const std::string& bitangent,
 const std::string& texture_coordinates,
 const std::string& bone_ids, const std::string& bone_weights) :
   position(position), normal(normal),
   tangent(tangent), bitangent(bitangent),
   texture_coordinates(texture_coordinates),
   bone_ids(bone_ids), bone_weights(bone_weights)
{}

UniformVariable::UniformVariable(
 const std::string& model_transform,
 const std::string& view_transform,
 const std::string& perspective_transform,
 const std::string& bones,
 const std::string& camera_position,
 const std::string& light_count,
 const std::string& light_position,
 const std::string& light_diffuse,
 const std::string& light_specular,
 const std::string& ambient_color,
 const std::string& diffuse_color,
 const std::string& specular_color,
 const std::string& shininess_value,
 const std::string& ambient_base_color,
 const std::string& diffuse_base_color,
 const std::string& specular_base_color,
 const std::string& shininess_base_value,
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
 const std::string& shininess_texture) :
    model_transform(model_transform),
    view_transform(view_transform),
    perspective_transform(perspective_transform),
    bones(bones),
    camera_position(camera_position),
    light_count(light_count),
    light_position(light_position),
    light_diffuse(light_diffuse),
    light_specular(light_specular),
    ambient_color(ambient_color),
    diffuse_color(diffuse_color),
    specular_color(specular_color),
    shininess_value(shininess_value),
    ambient_base_color(ambient_base_color),
    diffuse_base_color(diffuse_base_color),
    specular_base_color(specular_base_color),
    shininess_base_value(shininess_base_value),
    has_displacement_texture(has_displacement_texture),
    has_normal_texture(has_normal_texture),
    has_ambient_texture(has_ambient_texture),
    has_diffuse_texture(has_diffuse_texture),
    has_specular_texture(has_specular_texture),
    has_shininess_texture(has_shininess_texture),
    displacement_texture(displacement_texture),
    normal_texture(normal_texture),
    ambient_texture(ambient_texture),
    diffuse_texture(diffuse_texture),
    specular_texture(specular_texture),
    shininess_texture(shininess_texture)
{}

ShaderProgram::LinkFailure::LinkFailure(const std::string& error) :
   std::exception(), _error(error)
{}

const char* ShaderProgram::LinkFailure::what() const noexcept {
   return this->_error.data();
}

ShaderProgram::VariableAllocationFailure::VariableAllocationFailure(
 const std::string& variable) :
   std::exception(), _variable(variable)
{}

const char* ShaderProgram::VariableAllocationFailure::what() const noexcept {
   return this->_variable.data();
}

ShaderProgram::VariableReferenceFailure::VariableReferenceFailure(
 const std::string& variable) :
   std::exception(), _variable(variable)
{}

const char* ShaderProgram::VariableReferenceFailure::what() const noexcept {
   return this->_variable.data();
}

ShaderProgram::ShaderProgram(const Shaders& shaders,
 const UniformVariable& vars) :
   _shaders(shaders), _vars(vars)
{
   this->_handle = glCreateProgram();
}

/* virtual */ ShaderProgram::~ShaderProgram() {
   glDeleteProgram(this->_handle);
}

const ShaderProgram::Shaders& ShaderProgram::getShaders() const {
   return this->_shaders;
}

GLuint ShaderProgram::getHandle() const {
   return this->_handle;
}

void ShaderProgram::link() const {
   for (auto& shader : this->_shaders) {
      glAttachShader(this->_handle, shader->getHandle());
   }

   glLinkProgram(this->_handle);

   GLint isLinked = 0;
   glGetProgramiv(this->_handle, GL_LINK_STATUS, &isLinked);
   if (isLinked == GL_FALSE) {
      GLint maxLength = 0;
      glGetProgramiv(this->_handle, GL_INFO_LOG_LENGTH, &maxLength);

      std::vector< GLchar > errorLog(maxLength);
      glGetProgramInfoLog(this->_handle, maxLength, &maxLength, errorLog.data());

      glDeleteProgram(this->_handle);
      for (auto& shader : this->_shaders) {
         glDeleteShader(shader->getHandle());
      }

      throw LinkFailure(errorLog.data());
   }

   for (auto& shader : this->_shaders) {
      glDetachShader(this->_handle, shader->getHandle());
   }
}

void ShaderProgram::use() const {
   glUseProgram(this->_handle);
}

void ShaderProgram::createUniformVariable(const std::string& name) {
   GLint handle = glGetUniformLocation(this->_handle, name.c_str());
   if (handle < 0) {
      throw VariableAllocationFailure(name);
   }

   this->_variables[name] = handle;
}

GLint ShaderProgram::getVariableHandle(const std::string& name) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   return itr->second;
}

const UniformVariable& ShaderProgram::getVariableNames() const {
   return this->_vars;
}

void ShaderProgram::setUniformVariable1f(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniform1fv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable2f(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniform2fv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable3f(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniform3fv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable4f(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniform4fv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable1i(const std::string& name,
 const GLint* array, GLsizei length) const {
   glProgramUniform1iv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable2i(const std::string& name,
 const GLint* array, GLsizei length) const {
   glProgramUniform2iv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable3i(const std::string& name,
 const GLint* array, GLsizei length) const {
   glProgramUniform3iv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable4i(const std::string& name,
 const GLint* array, GLsizei length) const {
   glProgramUniform4iv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable1ui(const std::string& name,
 const GLuint* array, GLsizei length) const {
   glProgramUniform1uiv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable2ui(const std::string& name,
 const GLuint* array, GLsizei length) const {
   glProgramUniform2uiv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable3ui(const std::string& name,
 const GLuint* array, GLsizei length) const {
   glProgramUniform3uiv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariable4ui(const std::string& name,
 const GLuint* array, GLsizei length) const {
   glProgramUniform4uiv(this->_handle, this->getVariableHandle(name),
    length, array);
}

void ShaderProgram::setUniformVariableMatrix2(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniformMatrix2fv(this->_handle, this->getVariableHandle(name),
    length, GL_FALSE, array);
}

void ShaderProgram::setUniformVariableMatrix3(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniformMatrix3fv(this->_handle, this->getVariableHandle(name),
    length, GL_FALSE, array);
}

void ShaderProgram::setUniformVariableMatrix4(const std::string& name,
 const GLfloat* array, GLsizei length) const {
   glProgramUniformMatrix4fv(this->_handle, this->getVariableHandle(name),
    length, GL_FALSE, array);
}
