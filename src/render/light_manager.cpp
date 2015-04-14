#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/shader_program.hpp>

using namespace crash::render;

/* static */ const std::size_t LightManager::NUM_LIGHTS;

LightManager::LightManager(const LightManager& lightManager) :
   _positionHandle(lightManager._positionHandle),
   _diffuseHandle(lightManager._diffuseHandle),
   _specularHandle(lightManager._specularHandle),
   _position(lightManager._position),
   _diffuse(lightManager._diffuse),
   _specular(lightManager._specular)
{}

LightManager::LightManager(const std::string& positionHandle,
 const std::string& diffuseHandle, const std::string specularHandle) :
   _positionHandle(positionHandle), _diffuseHandle(diffuseHandle),
    _specularHandle(specularHandle)
{
   ::memset(this->_position.data(), 0,
    sizeof(glm::vec3) * LightManager::NUM_LIGHTS);
   ::memset(this->_diffuse.data(), 0,
    sizeof(glm::vec4) * LightManager::NUM_LIGHTS);
   ::memset(this->_specular.data(), 0,
    sizeof(glm::vec4) * LightManager::NUM_LIGHTS);
}

LightManager::LightManager(const std::string& positionHandle,
 const std::string& diffuseHandle, const std::string specularHandle,
 const std::vector< Light >& lights) :
   LightManager(positionHandle, diffuseHandle, specularHandle)
{
   for (unsigned int i = 0; i < lights.size(); ++i) {
      this->setLight(i, lights[i]);
   }
}

/* virtual */ LightManager::~LightManager() {}

Light LightManager::getLight(unsigned int index) const {
   return Light(this->_position[index], this->_diffuse[index],
    this->_specular[index]);
}

void LightManager::setLight(unsigned int index, const Light& light) {
   this->_position[index] = light.getPosition();
   this->_diffuse[index] = light.getDiffuse();
   this->_specular[index] = light.getSpecular();
}

void LightManager::setUniforms(const ShaderProgram& program) const {
   program.setUniformVariable3f(this->_positionHandle,
    reinterpret_cast< const GLfloat* >(this->_position.data()),
    LightManager::NUM_LIGHTS);
   program.setUniformVariable4f(this->_diffuseHandle,
    reinterpret_cast< const GLfloat* >(this->_diffuse.data()),
    LightManager::NUM_LIGHTS);
   program.setUniformVariable4f(this->_specularHandle,
    reinterpret_cast< const GLfloat* >(this->_specular.data()),
    LightManager::NUM_LIGHTS);
}

