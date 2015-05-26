#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <crash/engine/actor.hpp>
#include <crash/engine/driver.hpp>
#include <crash/space/boundable.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/renderable.hpp>
#include <crash/render/shader_program.hpp>

using namespace crash::engine;
using namespace crash::render;
using namespace crash::space;
using namespace crash::window;

Driver::Driver(const Driver& driver) :
   _boundingPartition(driver._boundingPartition),
    _camera(driver._camera),
    _lightManager(driver._lightManager),
    _window(driver._window),
    _shouldLoop(true),
    _renderBoundingGroups(false),
    _renderBoundingPartition(false),
    _updateTimer(),
    _renderTimer(),
    _updatesPerSecond(0.0f),
    _rendersPerSecond(0.0f)
{}

Driver::Driver(BoundingPartition* boundingPartition,
 Camera* camera, LightManager* lightManager,
 Window* window) :
   _boundingPartition(boundingPartition),
    _camera(camera),
    _lightManager(lightManager),
    _window(window),
    _shouldLoop(true),
    _renderBoundingGroups(false),
    _renderBoundingPartition(false),
    _updateTimer(),
    _renderTimer(),
    _updatesPerSecond(0.0f),
    _rendersPerSecond(0.0f)
{}

/* virtual */ Driver::~Driver() {}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

BoundingPartition* Driver::getBoundingPartition() const {
   return this->_boundingPartition;
}

void Driver::setBoundingPartition(BoundingPartition* boundingPartition) {
   this->_boundingPartition = boundingPartition;
}

Camera* Driver::getCamera() const {
   return this->_camera;
}

void Driver::setCamera(Camera* camera) {
   this->_camera = camera;
}

Window* Driver::getWindow() const {
   return this->_window;
}

void Driver::setWindow(Window* window) {
   this->_window = window;
}

bool Driver::getShouldLoop() const {
   return this->_shouldLoop && !this->_window->shouldClose();
}

void Driver::setShouldLoop(bool shouldLoop) {
   this->_shouldLoop = shouldLoop;
}

bool Driver::getRenderBoundingBoxes() const {
   return this->_renderBoundingBoxes;
}

void Driver::setRenderBoundingBoxes(bool renderBoundingBoxes) {
   this->_renderBoundingBoxes = renderBoundingBoxes;
}

bool Driver::getRenderBoundingGroups() const {
   return this->_renderBoundingGroups;
}

void Driver::setRenderBoundingGroups(bool renderBoundingGroups) {
   this->_renderBoundingGroups = renderBoundingGroups;
}

bool Driver::getRenderBoundingPartition() const {
   return this->_renderBoundingPartition;
}

void Driver::setRenderBoundingPartition(bool renderBoundingPartition) {
   this->_renderBoundingPartition = renderBoundingPartition;
}

////////////////////////////////////////////////////////////////////////////////
// Driver.
////////////////////////////////////////////////////////////////////////////////

void Driver::loop(float updateInterval, float renderInterval) {
   static const float smoothing = 0.01f;

   while (this->getShouldLoop()) {
      float updateElapsed = this->getUpdateTimerElapsed();
      if (updateElapsed > updateInterval) {
         {
            float ups = (1.0f / updateElapsed);
            float currentUps = ups * (1.0f - smoothing);
            float previousUps = this->_updatesPerSecond * smoothing;
            this->_updatesPerSecond = currentUps + previousUps;
         }

         this->resetUpdateTimer();
         this->update(updateElapsed);
      }

      float renderElapsed = this->getRenderTimerElapsed();
      if (renderElapsed > renderInterval) {
         {
            float rps = (1.0f / renderElapsed);
            float currentRps = rps * (1.0f - smoothing);
            float previousRps = this->_rendersPerSecond * smoothing;
            this->_rendersPerSecond = currentRps + previousRps;
         }

         this->resetRenderTimer();
         this->render(renderElapsed);
      }
   }
}

float Driver::getUpdatesPerSecond() const {
   return this->_updatesPerSecond;
}

float Driver::getRendersPerSecond() const {
   return this->_rendersPerSecond;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers.
////////////////////////////////////////////////////////////////////////////////

void Driver::update(float delta_t) {
   glfwPollEvents();


   std::vector< Boundable* > boundables =
    this->_boundingPartition->getBoundables();

   for (Boundable* boundable : boundables) {
      boundable->move(delta_t);
      this->_boundingPartition->update(boundable);
   }

   for (Boundable* boundable : boundables) {
      Actor* actor = dynamic_cast< Actor* >(boundable);
      if (actor == nullptr) {
         continue;
      }

      MeshInstance* instance = actor->getMeshInstance();
      instance->progressAnimations(delta_t);
   }
}

void Driver::render(float delta_t) const {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   std::vector< Boundable* > visibleBoundables =
    this->_boundingPartition->getVisibleElements(
    this->_camera->getViewFrustum());
   for (Boundable* boundable : visibleBoundables) {
      Renderable* renderable = dynamic_cast< Renderable* >(boundable);
      if (renderable == nullptr) {
         continue;
      }

      ShaderProgramPtr program =
       renderable->getMeshInstance()->getShaderProgram();
      const UniformVariable& uniforms = program->getVariableNames();

      program->use();

      glm::mat4 perspective = this->_camera->getPerspective();
      program->setUniformVariableMatrix4(uniforms.perspective_transform,
       glm::value_ptr(perspective), 1);

      glm::mat4 view = this->_camera->getLookAt();
      program->setUniformVariableMatrix4(uniforms.view_transform,
       glm::value_ptr(view), 1);

      glm::vec3 position = this->_camera->getPosition();
      program->setUniformVariable3f(uniforms.camera_position,
       glm::value_ptr(position), 1);

      this->_lightManager->setUniforms(*program);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_CULL_FACE);

      renderable->render(delta_t);

      if (Driver::BoundingCubeMeshInstance != nullptr &&
       this->getRenderBoundingBoxes()) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         glDisable(GL_CULL_FACE);

         glm::mat4 transform = boundable->getBoundingBox()->getTransform();
         Driver::BoundingCubeMeshInstance->render(transform, delta_t);
      }
   }

   if (Driver::BoundingCubeMeshInstance != nullptr) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDisable(GL_CULL_FACE);

      if (this->_renderBoundingPartition) {
         glm::mat4 transform = this->_boundingPartition->getTransform();
         Driver::BoundingCubeMeshInstance->render(transform, delta_t);
      }

      if (this->getRenderBoundingGroups()) {
         const auto groups = this->_boundingPartition->getBoundingGroups();
         for (const BoundingGroup& group : groups) {
            glm::mat4 transform = group.getTransform();
            Driver::BoundingCubeMeshInstance->render(transform, delta_t);
         }
      }
   }

   this->_window->swapBuffers();
}

float Driver::getUpdateTimerElapsed() const {
   return this->getTimerElapsed(this->_updateTimer);
}

float Driver::getRenderTimerElapsed() const {
   return this->getTimerElapsed(this->_renderTimer);
}

float Driver::getTimerElapsed(const boost::timer::cpu_timer& timer) const {
   return timer.elapsed().wall * 1E-9;
}

void Driver::resetUpdateTimer() {
   this->_updateTimer.start();
}

void Driver::resetRenderTimer() {
   this->_renderTimer.start();
}

/* static */ MeshInstancePtr Driver::BoundingCubeMeshInstance = nullptr;
