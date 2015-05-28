#pragma once

#include <exception>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <crash/render/light.hpp>

namespace crash {
namespace render {

class ShaderProgram;

class LightManager;
typedef std::shared_ptr< render::LightManager > LightManagerPtr;

class LightManager {
public:
   static const unsigned int MAX_NUM_LIGHTS;

   struct LightLimitExceeded : public std::exception {
      LightLimitExceeded();
      const char* what() const noexcept;
   };

   LightManager(const LightManager& lightManager);
   LightManager();
   LightManager(const std::vector< Light >& lights);
   virtual ~LightManager();

   unsigned int getLightCount() const;
   Light getLight(unsigned int index) const;
   std::vector< Light > getLights() const;

   void addLight(const Light& light);
   void removeLight(unsigned int index);
   void setLight(unsigned int index, const Light& light);
   void clearLights();

   void setUniforms(const ShaderProgram& program) const;

private:
   std::vector< glm::vec3 > _position;
   std::vector< glm::vec4 > _diffuse;
   std::vector< glm::vec4 > _specular;
};

} // namespace render
} // namespace crash
