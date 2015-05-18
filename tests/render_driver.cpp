#include <iostream>
#include <memory>
#include <string>
#include <boost/timer/timer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

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
#include <crash/common/util.hpp>
#include <crash/render/camera.hpp>
#include <crash/render/light.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/shader.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>
#include <crash/window/keyboard.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

using namespace crash::common;
using namespace crash::render;
using namespace crash::window;

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
std::shared_ptr< Camera > camera;
std::vector< MeshInstance > meshes;

glm::vec3 getStartingPosition();
glm::quat getStartingRotation();

Window initializeOpenGl();

std::tuple<
   std::shared_ptr< Camera >,
   std::shared_ptr< LightManager >,
   std::shared_ptr< ShaderProgram >,
   std::shared_ptr< AttributeVariable >,
   std::shared_ptr< UniformVariable >,
   std::shared_ptr< Mesh >
> initializeRender(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath,
 const boost::filesystem::path& meshPath);

void progressAnimations(float delta_t);

void render(
 float delta_t,
 const std::shared_ptr< Camera >& camera,
 const std::shared_ptr< LightManager >& lightManager,
 const std::shared_ptr< ShaderProgram >& program,
 const std::shared_ptr< UniformVariable >& uniforms);

void update(float delta_t);

void keyCb(const Window& window, int key, int, int action, int mods);

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
   }
   const std::string file = argv[1];

   Window window = initializeOpenGl();
   window.setKeyCallback(keyCb);
   window.setVisible(true);
   Keyboard keyboard(window);

   std::shared_ptr< LightManager > lightManager;
   std::shared_ptr< ShaderProgram > program;
   std::shared_ptr< AttributeVariable > attributes;
   std::shared_ptr< UniformVariable > uniforms;
   std::shared_ptr< Mesh > mesh;
   try {
      std::tie(camera, lightManager, program, attributes, uniforms, mesh) =
       initializeRender(
       boost::filesystem::path("tests/render/vertex.glsl"),
       boost::filesystem::path("tests/render/fragment.glsl"),
       boost::filesystem::path(file));
   } catch (Mesh::SceneImportFailure e) {
      std::cerr << "Scene Import Failure: " << e.error << std::endl;
      return 2;
   } catch (Shader::CompileFailure e) {
      std::cerr << "Shader Compile Failure: " << e.error << std::endl;
      return 3;
   } catch (ShaderProgram::LinkFailure e) {
      std::cerr << "Shader Program Link Failure: " << e.error << std::endl;
      return 4;
   } catch (ShaderProgram::VariableAllocationFailure e) {
      std::cerr << "Shader Program Variable Allocation Failure: " <<
       e.error << std::endl;
      return 5;
   }

   meshes.push_back(MeshInstance(*mesh,
    ColorUnit(glm::vec4(1.0f), glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), glm::vec4(1.0f), 1.0f),
    Transformer(glm::vec3(0.0f, 0.0f, 1.0f), NO_ROTATION, UNIT_SIZE,
    glm::vec3(), NO_ROTATION, glm::vec3(1.0f))));
   meshes.push_back(MeshInstance(*mesh,
    ColorUnit(glm::vec4(1.0f), glm::vec4(0.3f, 0.3f, 1.0f, 1.0f), glm::vec4(1.0f), 1.0f),
    Transformer(glm::vec3(0.0f, 0.0f, -1.0f),
    axisAngleToQuat(glm::vec4(Y_AXIS, glm::radians(180.0f))), UNIT_SIZE,
    glm::vec3(), NO_ROTATION, glm::vec3(1.0f))));

   boost::timer::cpu_timer renderTimer;
   boost::timer::cpu_timer updateTimer;
   const float renderInterval = 1.0f / 60.0f;
   const float updateInterval = 1.0f / 25.0f;
   while (!window.shouldClose()) {
      float renderElapsed = renderTimer.elapsed().wall * 1E-9;
      float updateElapsed = updateTimer.elapsed().wall * 1E-9;

      if (renderElapsed > renderInterval) {
         renderTimer.start();

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         render(renderElapsed, camera, lightManager, program, uniforms);
         window.swapBuffers();
      }

      if (updateElapsed > updateInterval) {
         updateTimer.start();
         glfwPollEvents();
         update(/* delta t */ updateElapsed);
      }
   }

   mesh->teardown();
   glUseProgram(0);

   return 0;
}

Window initializeOpenGl() {
   Window::setHint(GLFW_SAMPLES, 4);
   Window::setHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   Window::setHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   Window::setHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   Window::setHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   Window window(glm::ivec2(900, 600), "OpenGL Test", boost::none, boost::none);
   window.makeContextCurrent();
   window.swapInterval(1);

   glewExperimental = GL_TRUE;
   if (glewInit() != GLEW_OK) {
      std::cerr << "failed to initialize glew" << std::endl;
      throw 1;
   }

   glViewport(0, 0, 900, 600);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   // culling
   glCullFace(GL_BACK);

   // depth
   glEnable(GL_DEPTH_TEST);
   glClearDepth(1.0f);
   glDepthMask(GL_TRUE);
   glDepthFunc(GL_LEQUAL);

   return window;
}

std::tuple<
   std::shared_ptr< Camera >,
   std::shared_ptr< LightManager >,
   std::shared_ptr< ShaderProgram >,
   std::shared_ptr< AttributeVariable >,
   std::shared_ptr< UniformVariable >,
   std::shared_ptr< Mesh >
> initializeRender(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath,
 const boost::filesystem::path& meshPath) {
   ShaderProgram::Shaders shaders = {{
      std::make_shared< Shader >(vertexShaderPath, GL_VERTEX_SHADER),
      std::make_shared< Shader >(fragmentShaderPath, GL_FRAGMENT_SHADER),
   }};
   for (auto shader : shaders) {
      // @throws Shader::CompileFailure
      shader->compile();
   }

   std::shared_ptr< ShaderProgram > program =
    std::make_shared< ShaderProgram >(shaders);

   std::shared_ptr< AttributeVariable > attributes =
    std::make_shared< AttributeVariable >(
    "aPosition", "aNormal", "aTangent", "aBitangent",
    "aTexCoord", "aBoneIds", "aBoneWeights");

   // @throws Texture::ImportFailure
   std::shared_ptr< Mesh > mesh = std::make_shared< Mesh >(meshPath);
   mesh->initialize();
   mesh->bindAttributes(*program, *attributes);

   // @throws ShaderProgram::LinkFailure
   program->link();

   std::shared_ptr< UniformVariable > uniforms =
    std::make_shared< UniformVariable >(
    "uModelTransform",
    "uViewTransform",
    "uPerspectiveTransform",
    "uBones",
    "uCameraPosition",
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

   program->createUniformVariable(uniforms->model_transform);
   program->createUniformVariable(uniforms->view_transform);
   program->createUniformVariable(uniforms->perspective_transform);
   program->createUniformVariable(uniforms->bones);
   program->createUniformVariable(uniforms->camera_position);
   program->createUniformVariable(uniforms->light_position);
   program->createUniformVariable(uniforms->light_diffuse);
   program->createUniformVariable(uniforms->light_specular);
   program->createUniformVariable(uniforms->ambient_color);
   program->createUniformVariable(uniforms->diffuse_color);
   program->createUniformVariable(uniforms->specular_color);
   program->createUniformVariable(uniforms->shininess_value);
   program->createUniformVariable(uniforms->ambient_base_color);
   program->createUniformVariable(uniforms->diffuse_base_color);
   program->createUniformVariable(uniforms->specular_base_color);
   program->createUniformVariable(uniforms->shininess_base_value);
   program->createUniformVariable(uniforms->has_displacement_texture);
   program->createUniformVariable(uniforms->has_normal_texture);
   program->createUniformVariable(uniforms->has_ambient_texture);
   program->createUniformVariable(uniforms->has_diffuse_texture);
   program->createUniformVariable(uniforms->has_specular_texture);
   program->createUniformVariable(uniforms->has_shininess_texture);
   program->createUniformVariable(uniforms->displacement_texture);
   program->createUniformVariable(uniforms->normal_texture);
   program->createUniformVariable(uniforms->ambient_texture);
   program->createUniformVariable(uniforms->diffuse_texture);
   program->createUniformVariable(uniforms->specular_texture);
   program->createUniformVariable(uniforms->shininess_texture);

   std::shared_ptr< Camera > camera = std::make_shared< Camera >(
    getStartingPosition(), getStartingRotation(),
    /* fov-y */ glm::radians(60.0f), /* aspect */ 3.0f / 2.0f,
    /* near */ 0.01f, /* far */ 100.0f);

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

   std::shared_ptr< LightManager > lm = std::make_shared< LightManager >(
    uniforms->light_position, uniforms->light_diffuse,
    uniforms->light_specular, lights);

   return std::make_tuple(camera, lm, program, attributes, uniforms, mesh);
}

glm::vec3 getStartingPosition() {
   return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::quat getStartingRotation() {
   return NO_ROTATION;
}

void progressAnimations(float delta_t) {
   for (auto& instance : meshes) {
      instance.progressAnimations(delta_t);
   }

   for (auto& instance : meshes) {
      auto& mesh = instance.getMesh();
      auto& animations = mesh.getAnimations();
      auto& animationProgress = instance.getAnimationProgress();

      for (unsigned int i = 0; i < animations.size(); ++i) {
         auto& progress = animationProgress[i];
         auto& animation = animations[i];
         if (progress.active && progress.progress > animation.getDuration()) {
            std::cout << "animation ended" << std::endl;
            instance.stopAnimation(i);
         }
      }
   }
}

void render(
 float delta_t,
 const std::shared_ptr< Camera >& camera,
 const std::shared_ptr< LightManager >& lightManager,
 const std::shared_ptr< ShaderProgram >& program,
 const std::shared_ptr< UniformVariable >& uniforms) {
   program->use();

   auto perspective = camera->getPerspective();
   auto view = camera->getLookAt();
   program->setUniformVariableMatrix4(uniforms->perspective_transform,
    glm::value_ptr(perspective), 1);
   program->setUniformVariableMatrix4(uniforms->view_transform,
    glm::value_ptr(view), 1);

   program->setUniformVariable3f(uniforms->camera_position,
    glm::value_ptr(camera->getPosition()), 1);

   lightManager->setUniforms(*program);

   for (auto& mesh : meshes) {
      mesh.render(*program, *uniforms, glm::mat4(), delta_t);
   }
}

static glm::vec3 getTranslationalVelocity() {
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

static glm::quat getRotationalVelocity() {
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

void update(float delta_t) {
   camera->setTranslationalVelocity(getTranslationalVelocity());
   camera->setRotationalVelocity(getRotationalVelocity());

   camera->move(delta_t);

   progressAnimations(delta_t);
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
   } else if (key == GLFW_KEY_1) {
      if (control) {
         for (auto& mesh : meshes) {
            std::cout << "animation started" << std::endl;
            mesh.startAnimation(0);
         }
      }
   }
}
