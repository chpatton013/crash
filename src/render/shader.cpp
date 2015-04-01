#include <fstream>
#include <string>
#include <vector>
#include <crash/render/shader.hpp>
#include <crash/render/util.hpp>

static std::string readFile(const std::string& file);

using namespace crash::render;

Shader::CompileFailure::CompileFailure(const boost::filesystem::path& path,
 const std::string& error) :
   path(path), error(error)
{}

Shader::Shader(const boost::filesystem::path& path, const GLenum& type) :
   _path(path), _type(type)
{
   std::string contents = readFile(path.string());
   const char* contentsCharArray = contents.c_str();

   GLuint handle = glCreateShader(type);
   glShaderSource(handle, 1, &contentsCharArray, nullptr);

   this->_handle = handle;
}

/* virtual */ Shader::~Shader() {
   glDeleteShader(this->_handle);
}

const boost::filesystem::path& Shader::getPath() const {
   return this->_path;
}

GLuint Shader::getHandle() const {
   return this->_handle;
}

GLenum Shader::getType() const {
   return this->_type;
}

void Shader::compile() const {
   glCompileShader(this->_handle);

   GLint success = GL_FALSE;
   glGetShaderiv(this->_handle, GL_COMPILE_STATUS, &success);
   if (success == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(this->_handle, GL_INFO_LOG_LENGTH, &maxLength);

      std::vector< GLchar > errorLog(maxLength);
      glGetShaderInfoLog(this->_handle, maxLength, &maxLength, errorLog.data());

      glDeleteShader(this->_handle);

      throw CompileFailure(this->_path, errorLog.data());
   }
}

/* static */ std::string readFile(const std::string& file) {
   std::string accumulator;

   std::ifstream stream(file);
   std::string line;
   while(std::getline(stream, line)) {
      accumulator += line + "\n";
   }

   return accumulator;
}
