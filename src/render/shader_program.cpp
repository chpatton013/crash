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

GLint ShaderProgram::getVariableHandle(const std::string& name) const {
   auto itr = this->_variables.find(name);
   if (itr == this->_variables.end()) {
      throw VariableReferenceFailure(name);
   }

   return itr->second;
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
