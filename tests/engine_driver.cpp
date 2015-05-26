#include <iostream>
#include <string>
#include <vector>

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

controls_t controls;
CameraPtr camera;
BoundingPartitionPtr boundingPartition;
DriverPtr driver;

void closeCb(Window& window);
void keyCb(const Window& window, int key, int, int action, int mods);
void mouseButtonCb(const Window&, int, int action, int mods);
glm::vec3 getStartingPosition();
glm::quat getStartingRotation();
glm::vec3 getTranslationalVelocity();
glm::quat getRotationalVelocity();
void setVisibleElementsColor(const glm::vec4& color);
void toggleRenderBoundingPartition();
void toggleRenderBoundingGroups();
void toggleRenderBoundingBoxes();

WindowPtr getWindow();
MeshPtr getMesh(const boost::filesystem::path& path);
MeshPtr getCubeMesh(const boost::filesystem::path& path);
ShaderProgramPtr getShaderProgram(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath);
void linkShaderProgram(const ShaderProgramPtr& program,
 const std::vector< MeshPtr >& meshes);
std::vector< ActorPtr > getActors(const MeshPtr& mesh,
 const ShaderProgramPtr& program, const glm::vec3& dimensions,
 unsigned int numActors);
BoundingPartitionPtr getBoundingPartition(
 const std::vector< ActorPtr >& actors,
 const glm::vec3& dimensions, const glm::ivec3& partitions);
CameraPtr getCamera();
LightManagerPtr getLightManager(const ShaderProgramPtr& program);

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
   }
   const std::string meshFile = argv[1];
   const std::string cubeFile = "/Users/cpatton/Desktop/cube/cube.obj";

   WindowPtr window;
   try {
      window = getWindow();
   } catch (Mesh::SceneImportFailure e) {
      std::cerr << "GLEW Initialization Failure" << std::endl;
      return 2;
   }

   MeshPtr mesh;
   MeshPtr cube;
   try {
      mesh = getMesh(boost::filesystem::path(meshFile));
      mesh->translate(glm::vec3(0.0f, -0.4f, -0.5f));
      cube = getMesh(boost::filesystem::path(cubeFile));
   } catch (Mesh::SceneImportFailure e) {
      std::cerr << "Scene Import Failure: " << e.what() << std::endl;
      return 3;
   } catch (Texture::ImportFailure e) {
      std::cerr << "Texture Import Failure: " << e.what() << std::endl;
      return 4;
   }
   mesh->initialize();
   cube->initialize();

   ShaderProgramPtr program;
   try {
      program = getShaderProgram(
       boost::filesystem::path("tests/render/vertex.glsl"),
       boost::filesystem::path("tests/render/fragment.glsl"));
   } catch (Shader::CompileFailure e) {
      std::cerr << "Shader Compile Failure: " << e.what() << std::endl;
      return 5;
   }

   std::vector< MeshPtr > meshes = {{ mesh, cube }};
   try {
      linkShaderProgram(program, meshes);
   } catch (ShaderProgram::LinkFailure e) {
      std::cerr << "Shader Program Link Failure: " << e.what() << std::endl;
      return 6;
   } catch (ShaderProgram::VariableAllocationFailure e) {
      std::cerr << "Shader Program Variable Allocation Failure: " <<
       e.what() << std::endl;
      return 7;
   }

   camera = getCamera();

   glm::vec3 dimensions(10.0f);
   glm::ivec3 partitions(2);
   std::vector< ActorPtr > actors = getActors(mesh, program, dimensions, 10);
   boundingPartition = getBoundingPartition(actors, dimensions, partitions);

   boundingPartition->add(camera.get());

   LightManagerPtr lightManager = getLightManager(program);

   driver = std::make_shared< Driver >(
    boundingPartition.get(), camera.get(), lightManager.get(), window.get());

   Driver::BoundingCubeMeshInstance = std::make_shared< MeshInstance >(
    *cube, ColorUnit(glm::vec4(), glm::vec4(), glm::vec4(), 0.0f), program);

   const float updateInterval = 1.0f / 25.0f;
   const float renderInterval = 1.0f / 60.0f;
   driver->loop(updateInterval, renderInterval);

   mesh->teardown();
   cube->teardown();

   return 0;
}

void closeCb(Window& window) {
   window.setShouldClose(true);
}

void keyCb(const Window& window, int key, int, int action, int mods) {
   static const glm::vec4 WHITE(0.7f, 0.7f, 0.7f, 1.0f);
   static const glm::vec4   RED(0.7f, 0.0f, 0.0f, 1.0f);
   static const glm::vec4 GREEN(0.0f, 0.7f, 0.0f, 1.0f);
   static const glm::vec4  BLUE(0.0f, 0.0f, 0.7f, 1.0f);

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

glm::vec3 getStartingPosition() {
   return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::quat getStartingRotation() {
   return NO_ROTATION;
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
    boundingPartition->getVisibleElements(camera->getViewFrustum());
   for (Boundable* boundable : visibleBoundables) {
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
 const boost::filesystem::path& fragmentShaderPath) {
   ShaderProgram::Shaders shaders = {{
      std::make_shared< Shader >(vertexShaderPath, GL_VERTEX_SHADER),
      std::make_shared< Shader >(fragmentShaderPath, GL_FRAGMENT_SHADER),
   }};
   for (auto shader : shaders) {
      // @throws Shader::CompileFailure
      shader->compile();
   }

   UniformVariable uniforms(
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
    "uShininessTexture");

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
 const ShaderProgramPtr& program, const glm::vec3& dimensions,
 unsigned int numActors) {
   std::vector< ActorPtr > actors;
   actors.reserve(numActors);

   glm::vec3 minDims = -dimensions * 0.5f;
   glm::vec3 maxDims = dimensions * 0.5f;

   for (unsigned int i = 0; i < numActors; ++i) {
      glm::vec4 ambient(1.0f);
      glm::vec4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
      glm::vec4 specular(1.0f);
      float shininess = 1.0f;
      ColorUnit color(ambient, diffuse, specular, shininess);
      MeshInstance instance(*mesh, color, program);

      glm::vec3 position = glm::vec3(
       rand_float(minDims.x, maxDims.x),
       rand_float(minDims.y, maxDims.y),
       rand_float(minDims.z, maxDims.z));

      glm::vec3 axis = Y_AXIS;
      float angle = glm::radians(180.0f);
      glm::quat orientation = axisAngleToQuat(glm::vec4(axis, angle));

      BoundingBox boundingBox(
       Transformer(position, orientation, UNIT_SIZE,
       glm::vec3(), NO_ROTATION, glm::vec3()));

      actors.push_back(std::make_shared< Actor >(boundingBox, instance));
   }

   return actors;
}

BoundingPartitionPtr getBoundingPartition(
 const std::vector< ActorPtr >& actors,
 const glm::vec3& dimensions, const glm::ivec3& partitions) {
   Transformer transformer(
    ORIGIN, NO_ROTATION, dimensions,
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
    /* near */ 0.01f, /* far */ 100.0f);
}

LightManagerPtr getLightManager(const ShaderProgramPtr& program) {
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

   const UniformVariable& uniforms = program->getVariableNames();
   return std::make_shared< LightManager >(
    uniforms.light_count, uniforms.light_position,
    uniforms.light_diffuse, uniforms.light_specular, lights);
}
