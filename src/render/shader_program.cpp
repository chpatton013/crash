#include <glm/gtc/type_ptr.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>

using namespace crash::render;

ShaderProgram::LinkFailure::LinkFailure(const std::string& error) :
   error(error)
{}

ShaderProgram::VariableAllocationFailure::VariableAllocationFailure(
 const std::string& error) :
   error(error)
{}

ShaderProgram::VariableReferenceFailure::VariableReferenceFailure(
 const std::string& error) :
   error(error)
{}

ShaderProgram::ShaderProgram(const Shaders& shaders) :
   _shaders(shaders)
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

void ShaderProgram::setUniformVariable(const std::string& name,
 GLboolean value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform1ui(this->_handle, itr->second, value);
}

void ShaderProgram::setUniformVariable(const std::string& name,
 GLint value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform1i(this->_handle, itr->second, value);
}

void ShaderProgram::setUniformVariable(const std::string& name,
 GLuint value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform1ui(this->_handle, itr->second, value);
}

void ShaderProgram::setUniformVariable(const std::string& name,
 GLfloat value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform1f(this->_handle, itr->second, value);
}

void ShaderProgram::setUniformVariable(const std::string& name,
 const glm::vec3& value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform3fv(this->_handle, itr->second, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniformVariable(const std::string& name,
 const glm::vec4& value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniform4fv(this->_handle, itr->second, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniformVariable(const std::string& name,
 const glm::mat4& value) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   glProgramUniformMatrix4fv(this->_handle, itr->second, 1, GL_FALSE,
    glm::value_ptr(value));
}
