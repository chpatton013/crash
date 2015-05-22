#pragma once

#include <exception>
#include <memory>
#include <boost/filesystem/path.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

namespace crash {
namespace render {

class Shader;
typedef std::shared_ptr< Shader > ShaderPtr;

class Shader {
public:
   Shader(const boost::filesystem::path& path, const GLenum& type);
   virtual ~Shader();

   const boost::filesystem::path& getPath() const;
   GLuint getHandle() const;
   GLenum getType() const;

   void compile() const;

   class CompileFailure : public std::exception {
   public:
      CompileFailure(const boost::filesystem::path& path,
       const std::string& error);
      const char* what() const noexcept;

   private:
      boost::filesystem::path _path;
      std::string _error;
   };

private:
   boost::filesystem::path _path;
   GLuint _handle;
   GLenum _type;
};

} // namespace render
} // namespace crash
