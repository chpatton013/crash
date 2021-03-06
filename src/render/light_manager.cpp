#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/shader_program.hpp>

using namespace crash::render;

LightManager::LightLimitExceeded::LightLimitExceeded() :
   std::exception()
{}

const char* LightManager::LightLimitExceeded::what() const noexcept {
   return "Global light limit exceeded";
}

LightManager::LightManager(const LightManager& lightManager) :
   _position(lightManager._position),
   _diffuse(lightManager._diffuse),
   _specular(lightManager._specular)
{}

LightManager::LightManager() :
    _position(), _diffuse(), _specular()
{}

LightManager::LightManager(const std::vector< Light >& lights) :
   LightManager()
{
   for (const Light& light : lights) {
      this->addLight(light);
   }
}

/* virtual */ LightManager::~LightManager() {}

unsigned int LightManager::getLightCount() const {
   return this->_position.size();
}

Light LightManager::getLight(unsigned int index) const {
   return Light(this->_position[index], this->_diffuse[index],
    this->_specular[index]);
}

std::vector< Light > LightManager::getLights() const {
   std::vector< Light > lights;
   for (unsigned int i = 0; i < this->getLightCount(); ++i) {
      lights.push_back(this->getLight(i));
   }
   return lights;
}

void LightManager::addLight(const Light& light) {
   if (this->getLightCount() >= LightManager::MAX_NUM_LIGHTS) {
      throw LightManager::LightLimitExceeded();
   }

   this->_position.push_back(light.getPosition());
   this->_diffuse.push_back(light.getDiffuse());
   this->_specular.push_back(light.getSpecular());
}

void LightManager::removeLight(unsigned int index) {
   this->_position.erase(this->_position.begin() + index);
   this->_diffuse.erase(this->_diffuse.begin() + index);
   this->_specular.erase(this->_specular.begin() + index);
}

void LightManager::setLight(unsigned int index, const Light& light) {
   this->_position[index] = light.getPosition();
   this->_diffuse[index] = light.getDiffuse();
   this->_specular[index] = light.getSpecular();
}

void LightManager::clearLights() {
   this->_position.clear();
   this->_diffuse.clear();
   this->_specular.clear();
}

void LightManager::setUniforms(const ShaderProgram& program) const {
   unsigned int numLights = this->getLightCount();
   program.setUniformVariable1ui(program.getVariableNames().light_count,
    reinterpret_cast< const GLuint* >(&numLights), 1);
   program.setUniformVariable3f(program.getVariableNames().light_position,
    reinterpret_cast< const GLfloat* >(this->_position.data()), numLights);
   program.setUniformVariable4f(program.getVariableNames().light_diffuse,
    reinterpret_cast< const GLfloat* >(this->_diffuse.data()), numLights);
   program.setUniformVariable4f(program.getVariableNames().light_specular,
    reinterpret_cast< const GLfloat* >(this->_specular.data()), numLights);
}

/* static */ const unsigned int LightManager::MAX_NUM_LIGHTS = 8;
