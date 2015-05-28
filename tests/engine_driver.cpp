#include <iostream>
#include <string>
#include <vector>
#include <glm/gtx/vector_angle.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <crash/common/arithmetic.hpp>
#include <crash/common/symbols.hpp>
#include <crash/common/transformer.hpp>
#include <crash/common/util.hpp>
#include <crash/engine/actor.hpp>
#include <crash/engine/camera.hpp>
#include <crash/engine/driver.hpp>
#include <crash/render/light.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/shader.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/space/bounding_partition.hpp>
#include <crash/space/collision.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

using namespace crash::common;
using namespace crash::engine;
using namespace crash::render;
using namespace crash::space;
using namespace crash::window;

struct GlewInitializationFailure {};

struct controls_t {
   bool move_left;
   bool move_right;
   bool move_up;
   bool move_down;
   bool move_forward;
   bool move_backward;
   bool turn_left;
   bool turn_right;
   bool lean_forward;
   bool lean_backward;
   bool roll_left;
   bool roll_right;
};

static const glm::vec4 WHITE(0.7f, 0.7f, 0.7f, 1.0f);
static const glm::vec4   RED(0.7f, 0.0f, 0.0f, 1.0f);
static const glm::vec4 GREEN(0.0f, 0.7f, 0.0f, 1.0f);
static const glm::vec4  BLUE(0.0f, 0.0f, 0.7f, 1.0f);

controls_t controls;
CameraPtr camera;
ActorPtr planeActor;
ActorPtr skyActor;
BoundingPartitionPtr boundingPartition;
DriverPtr driver;

struct Checkpoints {
   Checkpoints(const std::vector< glm::vec3 >& checkpoints, unsigned int progress) :
      checkpoints(checkpoints), progress(progress)
   {}
   std::vector< glm::vec3 > checkpoints;
   unsigned int progress;
};

std::map< Actor*, Checkpoints > actorCheckpoints;
std::tuple< glm::vec3, glm::quat > getCheckpointDirection(
 const Actor* actor, const Checkpoints& checkpoints);

void closeCb(Window& window);
void keyCb(const Window& window, int key, int, int action, int mods);
void mouseButtonCb(const Window&, int, int action, int mods);
void freezeCollisionCb(const Collision& collision);
void colorCollisionCb(const Collision& collision);
void updateCb(Boundable* boundable);
glm::vec3 getStartingPosition();
glm::quat getStartingRotation();
glm::vec3 getTranslationalVelocity();
glm::quat getRotationalVelocity();
void setColor(Actor* actor);
void setVisibleElementsColor(const glm::vec4& color);
void toggleRenderBoundingPartition();
void toggleRenderBoundingGroups();
void toggleRenderBoundingBoxes();

WindowPtr getWindow();
MeshPtr getMesh(const boost::filesystem::path& path);
MeshPtr getCubeMesh(const boost::filesystem::path& path);
ShaderProgramPtr getShaderProgram(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath,
 const UniformVariable& uniforms);
void linkShaderProgram(const ShaderProgramPtr& program,
 const std::vector< MeshPtr >& meshes);
std::vector< ActorPtr > getActors(const MeshPtr& mesh,
 const ShaderProgramPtr& program);
ActorPtr getActor(const MeshPtr& mesh, const ShaderProgramPtr& program,
 const Checkpoints& checkpoints);
BoundingPartitionPtr getBoundingPartition(
 const std::vector< ActorPtr >& actors,
 const glm::vec3& dimensions, const glm::ivec3& partitions);
CameraPtr getCamera();
ActorPtr getPlane(const MeshPtr& plane, const ShaderProgramPtr& program);
ActorPtr getSkybox(const MeshPtr& sky, const ShaderProgramPtr& program);
LightManagerPtr getLightManager();

int main() {
   const std::string meshFile = "/Users/cpatton/Desktop/batman/batman.dae";
   const std::string cubeFile = "/Users/cpatton/Desktop/cube/cube.obj";
   const std::string planeFile = "/Users/cpatton/Desktop/plane/plane.obj";
   const std::string skyFile = "/Users/cpatton/Desktop/skybox/skybox.obj";

   WindowPtr window;
   try {
      window = getWindow();
   } catch (Mesh::SceneImportFailure e) {
      std::cerr << "GLEW Initialization Failure" << std::endl;
      return 2;
   }

   MeshPtr mesh;
   MeshPtr cube;
   MeshPtr plane;
   MeshPtr sky;
   try {
      mesh = getMesh(boost::filesystem::path(meshFile));
      mesh->translate(glm::vec3(0.0f, -0.4f, -0.5f));
      cube = getMesh(boost::filesystem::path(cubeFile));
      plane = getMesh(boost::filesystem::path(planeFile));
      sky = getMesh(boost::filesystem::path(skyFile));
   } catch (Mesh::SceneImportFailure e) {
      std::cerr << "Scene Import Failure: " << e.what() << std::endl;
      return 3;
   } catch (Texture::ImportFailure e) {
      std::cerr << "Texture Import Failure: " << e.what() << std::endl;
      return 4;
   }
   mesh->initialize();
   cube->initialize();
   plane->initialize();
   sky->initialize();

   ShaderProgramPtr modelProgram;
   ShaderProgramPtr flatProgram;
   try {
      modelProgram = getShaderProgram(
       boost::filesystem::path("tests/render/model.vertex.glsl"),
       boost::filesystem::path("tests/render/model.fragment.glsl"),
       UniformVariable(
       "uModelTransform",
       "uViewTransform",
       "uPerspectiveTransform",
       "uBones",
       "uCameraPosition",
       "uLightCount",
       "uLightPosition",
       "uLightDiffuse",
       "uLightSpecular",
       "uAmbientColor",
       "uDiffuseColor",
       "uSpecularColor",
       "uShininessValue",
       "uAmbientBaseColor",
       "uDiffuseBaseColor",
       "uSpecularBaseColor",
       "uShininessBaseValue",
       "uHasDisplacementTexture",
       "uHasNormalTexture",
       "uHasAmbientTexture",
       "uHasDiffuseTexture",
       "uHasSpecularTexture",
       "uHasShininessTexture",
       "uDisplacementTexture",
       "uNormalTexture",
       "uAmbientTexture",
       "uDiffuseTexture",
       "uSpecularTexture",
       "uShininessTexture"));
      flatProgram = getShaderProgram(
       boost::filesystem::path("tests/render/flat.vertex.glsl"),
       boost::filesystem::path("tests/render/flat.fragment.glsl"),
       UniformVariable(
       "uModelTransform",
       "uViewTransform",
       "uPerspectiveTransform",
       "",
       "",
       "uLightCount",
       "uLightPosition",
       "uLightDiffuse",
       "",
       "uAmbientColor",
       "uDiffuseColor",
       "",
       "",
       "uAmbientBaseColor",
       "uDiffuseBaseColor",
       "",
       "",
       "",
       "",
       "uHasAmbientTexture",
       "uHasDiffuseTexture",
       "",
       "",
       "",
       "",
       "uAmbientTexture",
       "uDiffuseTexture",
       "",
       ""));
   } catch (Shader::CompileFailure e) {
      std::cerr << "Shader Compile Failure: " << e.what() << std::endl;
      return 5;
   }

   try {
      linkShaderProgram(modelProgram, {{ mesh }});
      linkShaderProgram(flatProgram, {{ cube, plane, sky }});
   } catch (ShaderProgram::LinkFailure e) {
      std::cerr << "Shader Program Link Failure: " << e.what() << std::endl;
      return 6;
   } catch (ShaderProgram::VariableAllocationFailure e) {
      std::cerr << "Shader Program Variable Allocation Failure: " <<
       e.what() << std::endl;
      return 7;
   }

   std::vector< ActorPtr > actors = getActors(mesh, modelProgram);
   glm::vec3 dimensions(100.0f);
   glm::ivec3 partitions(4);
   boundingPartition = getBoundingPartition(actors, dimensions, partitions);

   camera = getCamera();
   planeActor = getPlane(plane, flatProgram);
   skyActor = getSkybox(sky, flatProgram);

   boundingPartition->add(camera.get());
   boundingPartition->add(planeActor.get());
   boundingPartition->add(skyActor.get());

   LightManagerPtr lightManager = getLightManager();

   driver = std::make_shared< Driver >(
    boundingPartition.get(), camera.get(), lightManager.get(), window.get());

   driver->addCollisionCallback(freezeCollisionCb);
   driver->addCollisionCallback(colorCollisionCb);
   driver->addUpdateCallback(updateCb);

   Driver::BoundingCubeMeshInstance = std::make_shared< MeshInstance >(
    *cube, ColorUnit(glm::vec4(), glm::vec4(), glm::vec4(), 0.0f), modelProgram);

   const float updateInterval = 1.0f / 25.0f;
   const float renderInterval = 1.0f / 60.0f;
   driver->loop(updateInterval, renderInterval);

   mesh->teardown();
   cube->teardown();
   plane->teardown();
   sky->teardown();

   return 0;
}

std::tuple< glm::vec3, glm::quat > getCheckpointDirection(
 const Actor* actor, const Checkpoints& checkpoints) {
   glm::vec3 nextDestination = checkpoints.checkpoints[checkpoints.progress];
   glm::vec3 direction = glm::normalize(nextDestination - actor->getPosition());

   glm::vec3 translation = 2.0f * direction;

   float angle = glm::orientedAngle(FORWARD, direction, UP);
   glm::quat rotation = axisAngleToQuat(glm::vec4(UP, angle));

   return std::make_tuple(translation, rotation);
}

void closeCb(Window& window) {
   window.setShouldClose(true);
}

void keyCb(const Window& window, int key, int, int action, int mods) {
   if (action == GLFW_REPEAT) {
      return;
   }

   if (key == GLFW_KEY_ESCAPE) {
      window.setShouldClose(true);
      return;
   }

   bool control = action == GLFW_PRESS;

   if (key == GLFW_KEY_W) {
      controls.move_forward = control;
   } else if (key == GLFW_KEY_S) {
      controls.move_backward = control;
   } else if (key == GLFW_KEY_A) {
      controls.turn_left = control;
   } else if (key == GLFW_KEY_D) {
      controls.turn_right = control;
   } else if (key == GLFW_KEY_Q) {
      controls.move_left = control;
   } else if (key == GLFW_KEY_E) {
      controls.move_right = control;
   } else if (key == GLFW_KEY_I) {
      controls.lean_forward = control;
   } else if (key == GLFW_KEY_K) {
      controls.lean_backward = control;
   } else if (key == GLFW_KEY_J) {
      controls.roll_left = control;
   } else if (key == GLFW_KEY_L) {
      controls.roll_right = control;
   } else if (key == GLFW_KEY_1) {
      if (control) {
         setVisibleElementsColor(RED);
      }
   } else if (key == GLFW_KEY_2) {
      if (control) {
         setVisibleElementsColor(GREEN);
      }
   } else if (key == GLFW_KEY_3) {
      if (control) {
         setVisibleElementsColor(BLUE);
      }
   } else if (key == GLFW_KEY_4) {
      if (control) {
         setVisibleElementsColor(WHITE);
      }
   } else if (key == GLFW_KEY_SPACE) {
      if (mods & GLFW_MOD_SHIFT) {
         controls.move_down = control;
      } else {
         controls.move_up = control;
      }
   } else if (key == GLFW_KEY_0) {
      if (control) {
         camera->setPosition(getStartingPosition());
         camera->setOrientation(getStartingRotation());
      }
   }

   camera->setTranslationalVelocity(getTranslationalVelocity());
   camera->setRotationalVelocity(getRotationalVelocity());
}

void mouseButtonCb(const Window&, int, int action, int mods) {
   if (action != GLFW_PRESS) {
      return;
   }

   if (mods & GLFW_MOD_SHIFT) {
      toggleRenderBoundingPartition();
   } else if (mods & GLFW_MOD_CONTROL) {
      toggleRenderBoundingGroups();
   } else {
      toggleRenderBoundingBoxes();
   }
}

void freezeCollisionCb(const Collision& collision) {
   Boundable* other = nullptr;
   if (collision.getFirst() == camera.get()) {
      other = collision.getSecond();
   } else if (collision.getSecond() == camera.get()) {
      other = collision.getFirst();
   } else {
      return;
   }

   if (other == planeActor.get() || other == skyActor.get()) {
      return;
   }

   Actor* actor = dynamic_cast< Actor* >(other);
   if (actor != nullptr) {
      auto itr = actorCheckpoints.find(actor);
      if (itr != actorCheckpoints.end()) {
         actorCheckpoints.erase(itr);
         other->setTranslationalVelocity(glm::vec3());
      }
   }
}

void colorCollisionCb(const Collision& collision) {
   Actor* first = dynamic_cast< Actor* >(collision.getFirst());
   Actor* second = dynamic_cast< Actor* >(collision.getSecond());
   Actor* cameraActor = dynamic_cast< Actor* >(camera.get());

   if (first == cameraActor || first == planeActor.get() ||
    first == skyActor.get() || second == cameraActor ||
    second == planeActor.get() || second == skyActor.get()) {
      return;
   }

   setColor(first);
   setColor(second);
}

void setColor(Actor* actor) {
   Renderable* renderable = dynamic_cast< Renderable* >(actor);
   if (renderable == nullptr) {
      return;
   }

   MeshInstance* instance = renderable->getMeshInstance();
   ColorUnit color = instance->getColor();
   color.diffuse = RED;
   instance->setColor(color);
}

void updateCb(Boundable* boundable) {
   Actor* actor = dynamic_cast< Actor* >(boundable);
   if (actor == nullptr) {
      return;
   }

   auto itr = actorCheckpoints.find(actor);
   if (itr == actorCheckpoints.end()) {
      return;
   }

   Checkpoints& checkpoints = itr->second;
   glm::vec3 destination = checkpoints.checkpoints[checkpoints.progress];
   glm::vec3 difference = destination - actor->getPosition();
   if (glm::dot(difference, difference) < 0.5f) {
      checkpoints.progress =
       (checkpoints.progress + 1) % checkpoints.checkpoints.size();
   }

   glm::vec3 translation;
   glm::quat orientation;
   std::tie(translation, orientation) =
    getCheckpointDirection(actor, checkpoints);

   actor->setTranslationalVelocity(translation);
   actor->setOrientation(orientation);
}

glm::vec3 getStartingPosition() {
   return glm::vec3(10.0f);
}

glm::quat getStartingRotation() {
   return axisAngleToQuat(Y_AXIS, glm::radians(45.0f)) *
    axisAngleToQuat(X_AXIS, glm::radians(-30.0f));
}

glm::vec3 getTranslationalVelocity() {
   glm::vec3 t_vel(0.0f);

   if (controls.move_forward) {
      t_vel += camera->getForward();
   }
   if (controls.move_backward) {
      t_vel += -camera->getForward();
   }
   if (controls.move_left) {
      t_vel += -glm::normalize(glm::cross(
       camera->getForward(), camera->getUp()));
   }
   if (controls.move_right) {
      t_vel += glm::normalize(glm::cross(
       camera->getForward(), camera->getUp()));
   }
   if (controls.move_up) {
      t_vel += camera->getUp();
   }
   if (controls.move_down) {
      t_vel += -camera->getUp();
   }

   if (t_vel != glm::vec3(0.0f)) {
      t_vel = glm::normalize(t_vel) * 2.0f;
   }

   return t_vel;
}

glm::quat getRotationalVelocity() {
   glm::vec3 axis(0.0f);

   if (controls.turn_left) {
      axis += camera->getUp();
   }
   if (controls.turn_right) {
      axis += -camera->getUp();
   }
   if (controls.lean_forward) {
      axis += glm::normalize(glm::cross(
       camera->getUp(), camera->getForward()));
   }
   if (controls.lean_backward) {
      axis += -glm::normalize(glm::cross(
       camera->getUp(), camera->getForward()));
   }
   if (controls.roll_left) {
      axis += -camera->getForward();
   }
   if (controls.roll_right) {
      axis += camera->getForward();
   }

   if (axis == glm::vec3(0.0f)) {
      return NO_ROTATION;
   }

   return axisAngleToQuat(glm::vec4(glm::normalize(axis), glm::radians(90.0f)));
}

void setVisibleElementsColor(const glm::vec4& diffuse) {
   std::vector< Boundable* > visibleBoundables =
    boundingPartition->getBoundables();
   /* std::vector< Boundable* > visibleBoundables = */
   /*  boundingPartition->getVisibleElements(camera->getViewFrustum()); */
   for (Boundable* boundable : visibleBoundables) {
      if (boundable == planeActor.get() || boundable == skyActor.get()) {
         continue;
      }

      Renderable* renderable = dynamic_cast< Renderable* >(boundable);
      if (renderable == nullptr) {
         continue;
      }

      MeshInstance* instance = renderable->getMeshInstance();
      ColorUnit color = instance->getColor();
      color.diffuse = diffuse;
      instance->setColor(color);
   }
}

void toggleRenderBoundingPartition() {
   driver->setRenderBoundingPartition(!driver->getRenderBoundingPartition());
}

void toggleRenderBoundingGroups() {
   driver->setRenderBoundingGroups(!driver->getRenderBoundingGroups());
}

void toggleRenderBoundingBoxes() {
   driver->setRenderBoundingBoxes(!driver->getRenderBoundingBoxes());
}

WindowPtr getWindow() {
   Window::setHint(GLFW_SAMPLES, 4);
   Window::setHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   Window::setHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   Window::setHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   Window::setHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   WindowPtr window = std::make_shared< Window >(
    glm::ivec2(900, 600), "OpenGL Test", boost::none, boost::none);
   window->makeContextCurrent();
   window->swapInterval(1);

   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK) {
      std::cerr << "failed to initialize glew" << std::endl;
      throw 1;
   }

   glViewport(0, 0, 900, 600);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   // depth
   glEnable(GL_DEPTH_TEST);
   glClearDepth(1.0f);
   glDepthMask(GL_TRUE);
   glDepthFunc(GL_LEQUAL);

   window->setCloseCallback(closeCb);
   window->setKeyCallback(keyCb);
   window->setMouseButtonCallback(mouseButtonCb);
   window->setVisible(true);

   return window;
}

MeshPtr getMesh(const boost::filesystem::path& path) {
   std::shared_ptr< Mesh > mesh = std::make_shared< Mesh >(path);
   mesh->initialize();
   return mesh;
}

ShaderProgramPtr getShaderProgram(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath,
 const UniformVariable& uniforms) {
   ShaderProgram::Shaders shaders = {{
      std::make_shared< Shader >(vertexShaderPath, GL_VERTEX_SHADER),
      std::make_shared< Shader >(fragmentShaderPath, GL_FRAGMENT_SHADER),
   }};
   for (auto shader : shaders) {
      // @throws Shader::CompileFailure
      shader->compile();
   }

   return std::make_shared< ShaderProgram >(shaders, uniforms);
}

void linkShaderProgram(const ShaderProgramPtr& program,
 const std::vector< MeshPtr >& meshes) {
   AttributeVariable attributes(
    "aPosition",
    "aNormal",
    "aTangent",
    "aBitangent",
    "aTexCoord",
    "aBoneIds",
    "aBoneWeights");

   for (const MeshPtr& mesh : meshes) {
      mesh->bindAttributes(*program, attributes);
   }

   // @throws ShaderProgram::LinkFailure
   program->link();

   const UniformVariable& uniforms = program->getVariableNames();

   // @throws ShaderProgram::VariableAllocationFailure
   program->createUniformVariable(uniforms.model_transform);
   program->createUniformVariable(uniforms.view_transform);
   program->createUniformVariable(uniforms.perspective_transform);
   program->createUniformVariable(uniforms.bones);
   program->createUniformVariable(uniforms.camera_position);
   program->createUniformVariable(uniforms.light_count);
   program->createUniformVariable(uniforms.light_position);
   program->createUniformVariable(uniforms.light_diffuse);
   program->createUniformVariable(uniforms.light_specular);
   program->createUniformVariable(uniforms.ambient_color);
   program->createUniformVariable(uniforms.diffuse_color);
   program->createUniformVariable(uniforms.specular_color);
   program->createUniformVariable(uniforms.shininess_value);
   program->createUniformVariable(uniforms.ambient_base_color);
   program->createUniformVariable(uniforms.diffuse_base_color);
   program->createUniformVariable(uniforms.specular_base_color);
   program->createUniformVariable(uniforms.shininess_base_value);
   program->createUniformVariable(uniforms.has_displacement_texture);
   program->createUniformVariable(uniforms.has_normal_texture);
   program->createUniformVariable(uniforms.has_ambient_texture);
   program->createUniformVariable(uniforms.has_diffuse_texture);
   program->createUniformVariable(uniforms.has_specular_texture);
   program->createUniformVariable(uniforms.has_shininess_texture);
   program->createUniformVariable(uniforms.displacement_texture);
   program->createUniformVariable(uniforms.normal_texture);
   program->createUniformVariable(uniforms.ambient_texture);
   program->createUniformVariable(uniforms.diffuse_texture);
   program->createUniformVariable(uniforms.specular_texture);
   program->createUniformVariable(uniforms.shininess_texture);
}

std::vector< ActorPtr > getActors(const MeshPtr& mesh,
 const ShaderProgramPtr& program) {
   std::vector< ActorPtr > actors;

   { // nascar
      Checkpoints checkpoints({{
         glm::vec3(-8.0f, 0.5f, -5.0f),
         glm::vec3(-7.0f, 0.5f, -6.0f),
         glm::vec3( 7.0f, 0.5f, -6.0f),
         glm::vec3( 8.0f, 0.5f, -5.0f),
         glm::vec3( 7.0f, 0.5f, -4.0f),
         glm::vec3(-7.0f, 0.5f, -4.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // square
      Checkpoints checkpoints({{
         glm::vec3(-9.0f, 0.5f, -2.0f),
         glm::vec3(-5.0f, 0.5f, -2.0f),
         glm::vec3(-5.0f, 0.5f,  2.0f),
         glm::vec3(-9.0f, 0.5f,  2.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // diamond-tall
      Checkpoints checkpoints({{
         glm::vec3( 4.0f, 0.5f,  0.0f),
         glm::vec3( 6.0f, 0.5f, -6.0f),
         glm::vec3( 8.0f, 0.5f,  0.0f),
         glm::vec3( 6.0f, 0.5f,  6.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // diamond-fat
      Checkpoints checkpoints({{
         glm::vec3( 2.0f, 0.5f,  0.0f),
         glm::vec3( 6.0f, 0.5f, -2.0f),
         glm::vec3(10.0f, 0.5f,  0.0f),
         glm::vec3( 6.0f, 0.5f,  2.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // line top-left -> bottom-right
      Checkpoints checkpoints({{
         glm::vec3(-10.0f, 0.5f, -10.0f),
         glm::vec3( 10.0f, 0.5f,  10.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // line top-right -> bottom-left
      Checkpoints checkpoints({{
         glm::vec3( 10.0f, 0.5f, -10.0f),
         glm::vec3(-10.0f, 0.5f,  10.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // line bottom-right -> top-left
      Checkpoints checkpoints({{
         glm::vec3( 10.0f, 0.5f,  10.0f),
         glm::vec3(-10.0f, 0.5f, -10.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // line bottom-left -> top-right
      Checkpoints checkpoints({{
         glm::vec3(-10.0f, 0.5f,  10.0f),
         glm::vec3( 10.0f, 0.5f, -10.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   { // figure-eight
      Checkpoints checkpoints({{
         glm::vec3(-6.0f, 0.5f,  6.0f),
         glm::vec3(-6.0f, 0.5f, 10.0f),
         glm::vec3(-2.0f, 0.5f, 10.0f),
         glm::vec3( 2.0f, 0.5f,  6.0f),
         glm::vec3( 6.0f, 0.5f,  6.0f),
         glm::vec3( 6.0f, 0.5f, 10.0f),
         glm::vec3( 2.0f, 0.5f, 10.0f),
         glm::vec3(-2.0f, 0.5f,  6.0f),
      }}, 0);
      actors.push_back(getActor(mesh, program, checkpoints));
   }

   return actors;
}

ActorPtr getActor(const MeshPtr& mesh, const ShaderProgramPtr& program,
 const Checkpoints& checkpoints) {
   static const ColorUnit color(
    glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f);

   glm::vec3 position = ORIGIN;
   if (checkpoints.checkpoints.size() > 0) {
      position = checkpoints.checkpoints[0];
   }

   ActorPtr actor = std::make_shared< Actor >(BoundingBox(Transformer(
    position, NO_ROTATION, UNIT_SIZE,
    glm::vec3(), NO_ROTATION, glm::vec3())),
    MeshInstance(*mesh, color, program));

   actorCheckpoints.insert(std::make_pair(actor.get(), checkpoints));

   glm::vec3 translation;
   glm::quat orientation;
   std::tie(translation, orientation) =
    getCheckpointDirection(actor.get(), checkpoints);

   actor->setTranslationalVelocity(translation);
   actor->setOrientation(orientation);

   return actor;
}

BoundingPartitionPtr getBoundingPartition(
 const std::vector< ActorPtr >& actors,
 const glm::vec3& dimensions, const glm::ivec3& partitions) {
   Transformer transformer(
    glm::vec3(0.0f, dimensions.y * 0.5f, 0.0f), NO_ROTATION, dimensions,
    glm::vec3(), NO_ROTATION, glm::vec3());

   BoundingPartitionPtr boundingPartition =
    std::make_shared< BoundingPartition >(transformer, partitions);

   for (const ActorPtr& actor : actors) {
      boundingPartition->add(actor.get());
   }

   return boundingPartition;
}

CameraPtr getCamera() {
   return std::make_shared< Camera >(
    BoundingBox(Transformer(
    getStartingPosition(), getStartingRotation(), UNIT_SIZE,
    glm::vec3(), NO_ROTATION, glm::vec3())),
    /* fov-y */ glm::radians(60.0f), /* aspect */ 3.0f / 2.0f,
    /* near */ 0.01f, /* far */ 2000.0f);
}

ActorPtr getPlane(const MeshPtr& plane, const ShaderProgramPtr& program) {
   static const ColorUnit color(
    glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f);

   return std::make_shared< Actor >(
    BoundingBox(Transformer(
    ORIGIN, NO_ROTATION, glm::vec3(1000.0f),
    glm::vec3(), NO_ROTATION, glm::vec3())),
    MeshInstance(*plane, color, program));
}

ActorPtr getSkybox(const MeshPtr& sky, const ShaderProgramPtr& program) {
   static const ColorUnit color(
    glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f);

   return std::make_shared< Actor >(
    BoundingBox(Transformer(
    ORIGIN, NO_ROTATION, glm::vec3(1000.0f),
    glm::vec3(), NO_ROTATION, glm::vec3())),
    MeshInstance(*sky, color, program));
}

LightManagerPtr getLightManager() {
   std::vector< Light > lights = {{
      Light(
       /* position */ glm::vec3(1000.0f, 1000.0f, 1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(1000.0f, 1000.0f, -1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(1000.0f, -1000.0f, 1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(1000.0f, -1000.0f, -1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(-1000.0f, 1000.0f, 1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(-1000.0f, 1000.0f, -1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(-1000.0f, -1000.0f, 1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
      Light(
       /* position */ glm::vec3(-1000.0f, -1000.0f, -1000.0f),
       /* diffuse */ glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
       /* specular */ glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)),
   }};

   return std::make_shared< LightManager >(lights);
}
