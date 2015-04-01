#pragma once

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

class Shader {
public:
   Shader(const boost::filesystem::path& path, const GLenum& type);
   virtual ~Shader();

   const boost::filesystem::path& getPath() const;
   GLuint getHandle() const;
   GLenum getType() const;

   void compile() const;

   struct CompileFailure {
      CompileFailure(const boost::filesystem::path& path,
       const std::string& error);

      boost::filesystem::path path;
      std::string error;
   };

private:
   boost::filesystem::path _path;
   GLuint _handle;
   GLenum _type;
};

} // namespace render
} // namespace crash
