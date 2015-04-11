#pragma once

#include <array>
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

class LightManager {
public:
   static const std::size_t NUM_LIGHTS = 8;

   LightManager(const LightManager& lightManager);
   LightManager(const std::string& positionHandle,
    const std::string& diffuseHandle, const std::string specularHandle);
   LightManager(const std::string& positionHandle,
    const std::string& diffuseHandle, const std::string specularHandle,
    const std::vector< Light >& lights);
   virtual ~LightManager();

   Light getLight(unsigned int index) const;
   void setLight(unsigned int index, const Light& light);

   void createUniforms(ShaderProgram& program) const;
   void setUniforms(ShaderProgram& program) const;

private:
   const std::string _positionHandle;
   const std::string _diffuseHandle;
   const std::string _specularHandle;
   std::array< glm::vec3, LightManager::NUM_LIGHTS > _position;
   std::array< glm::vec4, LightManager::NUM_LIGHTS > _diffuse;
   std::array< glm::vec4, LightManager::NUM_LIGHTS > _specular;
};

} // namespace render
} // namespace crash
