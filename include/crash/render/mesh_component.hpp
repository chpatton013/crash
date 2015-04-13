#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

namespace crash {
namespace render {

class MatrixStack;
class ShaderProgram;
class Texture;
struct UniformVariable;

struct MeshComponent {
   static const glm::vec4 defaultAmbientColor;
   static const glm::vec4 defaultDiffuseColor;
   static const glm::vec4 defaultSpecularColor;
   static const float defaultShininess;

   MeshComponent(const MeshComponent& component);
   MeshComponent(const aiMesh* mesh, const aiMaterial* material,
    std::shared_ptr< Texture > texture, const GLuint& vao,
    const GLuint& vbo, const GLuint& ibo, const GLuint& tbo);

   void generateVertexArray();
   void generateVertexBuffer();
   void generateIndexBuffer();
   void generateTextureBuffer();

   void bindAttributes(const ShaderProgram& program) const;
   void render(const ShaderProgram& program, const UniformVariable& vars,
    const glm::mat4& transform) const;
   void setMaterialProperties();

   const aiMesh* mesh;
   const aiMaterial* material;
   glm::vec4 ambient;
   glm::vec4 diffuse;
   glm::vec4 specular;
   float shininess;
   bool twoSided;
   std::shared_ptr< Texture > texture;
   GLuint vao;
   GLuint vbo;
   GLuint ibo;
   GLuint tbo;
};

} // namespace render
} // namespace crash
