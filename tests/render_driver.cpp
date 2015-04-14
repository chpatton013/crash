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

#include <crash/math/symbols.hpp>
#include <crash/math/util.hpp>
#include <crash/render/light.hpp>
#include <crash/render/light_manager.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>
#include <crash/space/camera.hpp>
#include <crash/window/keyboard.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

using namespace crash::math;
using namespace crash::render;
using namespace crash::space;
using namespace crash::window;

Window initializeOpenGl();
std::tuple< std::shared_ptr< Camera >, std::vector< Light >,
 std::shared_ptr< ShaderProgram >, AttributeVariable, UniformVariable,
 std::shared_ptr< Mesh > > initializeRender(
 const boost::filesystem::path& vertexShaderPath,
 const boost::filesystem::path& fragmentShaderPath,
 const boost::filesystem::path& meshPath);
void render(std::shared_ptr< Camera > camera, const LightManager& lightManager,
 std::shared_ptr< ShaderProgram > program, const UniformVariable& vars,
 std::shared_ptr< Mesh > mesh, MatrixStack& stack);

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
   }
   const std::string file = argv[1];

   Window window = initializeOpenGl();
   window.setVisible(true);
   Keyboard keyboard(window);

   std::shared_ptr< Camera > camera;
   std::vector< Light > lights;
   std::shared_ptr< ShaderProgram > program;
   AttributeVariable attributes;
   UniformVariable uniforms;
   std::shared_ptr< Mesh > mesh;
   try {
      std::tie(camera, lights, program, attributes, uniforms, mesh) =
       initializeRender(
       boost::filesystem::path("tests/render/vertex.glsl"),
       boost::filesystem::path("tests/render/fragment.glsl"),
       boost::filesystem::path(file));
   } catch (Shader::CompileFailure e) {
      std::cerr << e.error << std::endl;
      return 2;
   } catch (ShaderProgram::LinkFailure e) {
      std::cerr << e.error << std::endl;
      return 3;
   }

   LightManager lightManager = LightManager(uniforms.light_position,
    uniforms.light_diffuse, uniforms.light_specular, lights);
   MatrixStack stack;

   lightManager.createUniforms(*program);
   program->createUniformVariable(uniforms.camera_position);
   program->createUniformVariable(uniforms.transform_matrix);

   program->createUniformVariable(uniforms.ambient_color);
   program->createUniformVariable(uniforms.diffuse_color);
   program->createUniformVariable(uniforms.specular_color);
   program->createUniformVariable(uniforms.shininess_value);

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

   boost::timer::cpu_timer timer;
   const boost::timer::nanosecond_type interval = 10E9 / 60.0f;
   while (!window.shouldClose()) {
      if (timer.elapsed().wall > interval) {
         timer.resume();

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         render(camera, lightManager, program, uniforms, mesh, stack);
         window.swapBuffers();
      }

      glfwPollEvents();
      if (keyboard.getKey(GLFW_KEY_ESCAPE)) {
         window.setShouldClose(true);
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

std::tuple< std::shared_ptr< Camera >, std::vector< Light >,
 std::shared_ptr< ShaderProgram >, AttributeVariable, UniformVariable,
 std::shared_ptr< Mesh > > initializeRender(
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

   AttributeVariable attributes = {
      "aPosition",
      "aNormal",
      "aTangent",
      "aBitangent",
      "aTexCoords",
   };

   // @throws Texture::ImportFailure
   std::shared_ptr< Mesh > mesh = std::make_shared< Mesh >(meshPath);
   mesh->initialize();
   mesh->bindAttributes(*program, attributes);

   // @throws ShaderProgram::LinkFailure
   program->link();

   UniformVariable uniforms = {
      "uTransformMatrix",
      "uCameraPosition",
      "uLightPosition",
      "uLightDiffuse",
      "uLightSpecular",
      "uAmbientColor",
      "uDiffuseColor",
      "uSpecularColor",
      "uShininessValue",
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
      "uShininessTexture",
   };

   std::shared_ptr< Camera > camera = std::make_shared< Camera >(
    /* position */ glm::vec3(0.0f, 0.0f, 1.0f),
    /* forward */ glm::vec3(0.0f, 0.0f, -1.0f),
    /* up */ glm::vec3(0.0f, 1.0f, 0.0f),
    /* fov-y */ glm::radians(60.0f), /* aspect */ 3.0f / 2.0f,
    /* near */ 0.1f, /* far */ 10.0f);

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

   return std::make_tuple(camera, lights, program, attributes, uniforms, mesh);
}

void render(std::shared_ptr< Camera > camera, const LightManager& lightManager,
 std::shared_ptr< ShaderProgram > program, const UniformVariable& vars,
 std::shared_ptr< Mesh > mesh, MatrixStack& stack) {
   auto perspective = camera->getPerspective();
   auto view = camera->getLookAt();
   stack.push(perspective * view);

   program->use();

   program->setUniformVariable3f(vars.camera_position,
    glm::value_ptr(camera->getPosition()), 1);

   lightManager.setUniforms(*program);

   mesh->render(*program, vars, stack);

   stack.pop(); // view, perspective
}
