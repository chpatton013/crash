#pragma once

#include <memory>
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
struct AttributeVariable;
struct UniformVariable;

struct MaterialUnit {
   MaterialUnit(const glm::vec4& ambient, const glm::vec4& diffuse,
    const glm::vec4& specular, float shininess, bool twoSided);
   glm::vec4 ambient;
   glm::vec4 diffuse;
   glm::vec4 specular;
   float shininess;
   bool twoSided;
};

struct GeometryUnit {
   GeometryUnit(const GLuint& vao, const GLuint& vbo, const GLuint& ibo);
   GLuint vao;
   GLuint vbo;
   GLuint ibo;
};

struct TextureUnit {
   TextureUnit(std::shared_ptr< Texture > texture, const GLuint& tbo,
    unsigned int index);
   std::shared_ptr< Texture > texture;
   GLuint tbo;
   unsigned int index;
};

struct MeshComponent {
   static const glm::vec4 defaultAmbientColor;
   static const glm::vec4 defaultDiffuseColor;
   static const glm::vec4 defaultSpecularColor;
   static const float defaultShininess;

   MeshComponent(const MeshComponent& component);
   MeshComponent(const aiMesh* mesh, const aiMaterial* material,
    const GeometryUnit& geomtryUnit, const TextureUnit& textureUnit);

   void generateVertexArray();
   void generateVertexBuffer();
   void generateIndexBuffer();
   void generateTextureBuffer();

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;
   void render(const ShaderProgram& program, const UniformVariable& vars,
    const glm::mat4& transform) const;

   static MaterialUnit extractMaterialUnit(const aiMaterial* material);

   const aiMesh* mesh;
   const aiMaterial* material;
   MaterialUnit materialUnit;
   GeometryUnit geometryUnit;
   TextureUnit textureUnit;
};

} // namespace render
} // namespace crash
