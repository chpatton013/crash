#include <iostream>
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
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>
#include <crash/space/camera.hpp>
#include <crash/window/keyboard.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

static const glm::vec4 defaultAmbientColor = glm::vec4(glm::vec3(0.4f), 1.0f);
static const glm::vec4 defaultDiffuseColor = glm::vec4(glm::vec3(0.7f), 1.0f);
static const glm::vec4 defaultSpecularColor = glm::vec4(glm::vec3(0.9f), 1.0f);
static const float defaultSpecularReflectivity = 250.0f;

using namespace crash::math;
using namespace crash::render;
using namespace crash::space;
using namespace crash::window;

Window initializeOpenGl();

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
   }
   const std::string file = argv[1];

   Window window = initializeOpenGl();
   window.setVisible(true);
   Keyboard keyboard(window);

   ShaderProgram::Shaders shaders = {{
      std::make_shared< Shader >(
       boost::filesystem::path("tests/render/vertex.glsl"),
       GL_VERTEX_SHADER),
      std::make_shared< Shader >(
       boost::filesystem::path("tests/render/fragment.glsl"),
       GL_FRAGMENT_SHADER),
   }};
   try {
      for (auto shader : shaders) {
         shader->compile();
      }
   } catch (Shader::CompileFailure e) {
      std::cerr << e.error << std::endl;
      return 2;
   }

   auto program = std::make_shared< ShaderProgram >(shaders);

   Mesh mesh = Mesh(boost::filesystem::path(file));
   mesh.initialize();
   mesh.bindAttributes(*program);

   try {
      program->link();
   } catch (ShaderProgram::LinkFailure e) {
      std::cerr << e.error << std::endl;
      return 3;
   }

   program->createUniformVariable("uMvpMatrix");
   program->createUniformVariable("uCameraPosition");

   program->createUniformVariable("uLightPosition");
   program->createUniformVariable("uLightIntensity");

   program->createUniformVariable("uDefaultAmbientColor");
   program->createUniformVariable("uDefaultDiffuseColor");
   program->createUniformVariable("uDefaultSpecularColor");
   program->createUniformVariable("uDefaultSpecularReflectivity");

   program->setUniformVariable4f("uDefaultAmbientColor", glm::value_ptr(defaultAmbientColor), 1);
   program->setUniformVariable4f("uDefaultDiffuseColor", glm::value_ptr(defaultDiffuseColor), 1);
   program->setUniformVariable4f("uDefaultSpecularColor", glm::value_ptr(defaultSpecularColor), 1);
   program->setUniformVariable1f("uDefaultSpecularReflectivity", &defaultSpecularReflectivity, 1);

   Camera camera(
    /* position */ glm::vec3(0.0f, 0.0f, 1.0f),
    /* forward */ glm::vec3(0.0f, 0.0f, -1.0f),
    /* up */ glm::vec3(0.0f, 1.0f, 0.0f),
    /* fov-y */ glm::radians(60.0f), /* aspect */ 3.0f / 2.0f,
    /* near */ 0.1f, /* far */ 10.0f);


   MatrixStack stack;

   boost::timer::cpu_timer timer;
   const boost::timer::nanosecond_type interval = 10E9 / 60.0f;
   while (!window.shouldClose()) {
      if (timer.elapsed().wall > interval) {
         timer.resume();

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         auto perspective = camera.getPerspective();
         auto view = camera.getLookAt();

         stack.push(perspective * view);

         float intensity = 0.5f;
         program->setUniformVariable3f("uCameraPosition",
          glm::value_ptr(camera.getPosition()), 1);
         program->setUniformVariable3f("uLightPosition",
          glm::value_ptr(glm::vec3(1000.0f, 1000.0f, 0.0f)), 1);
         program->setUniformVariable1f("uLightIntensity", &intensity, 1);

         program->use();

         mesh.render(*program, stack);

         stack.pop(); // view, perspective

         window.swapBuffers();
      }

      glfwPollEvents();
      if (keyboard.getKey(GLFW_KEY_ESCAPE)) {
         window.setShouldClose(true);
      }
   }

   mesh.teardown();
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
   glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

   // culling
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

   // depth
   glEnable(GL_DEPTH_TEST);
   glClearDepth(1.0f);
   glDepthMask(GL_TRUE);
   glDepthFunc(GL_LEQUAL);

   return window;
}
