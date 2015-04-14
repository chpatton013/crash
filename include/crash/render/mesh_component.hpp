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
    const GLint& index);
   std::shared_ptr< Texture > texture;
   GLuint tbo;
   GLint index;
};

struct TextureGroupUnit {
   TextureGroupUnit(const TextureUnit& diffuse);
   TextureUnit diffuse;
};

struct MeshComponent {
   static const glm::vec4 defaultAmbientColor;
   static const glm::vec4 defaultDiffuseColor;
   static const glm::vec4 defaultSpecularColor;
   static const float defaultShininess;

   MeshComponent(const MeshComponent& component);
   MeshComponent(const aiMesh* mesh, const aiMaterial* material,
    const GeometryUnit& geomtryUnit, const TextureGroupUnit& textureGroupUnit);

   void generateVertexArray();
   void generateVertexBuffer();
   void generateIndexBuffer();
   void generateTextureBuffers();
   void generateTextureBuffer(const TextureUnit& textureUnit);

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;
   void render(const ShaderProgram& program, const UniformVariable& vars,
    const glm::mat4& transform) const;

   void activateMaterial(const ShaderProgram& program,
    const UniformVariable& vars) const;
   void activateTextures(const ShaderProgram& program,
    const UniformVariable& vars) const;
   void activateTexture(const ShaderProgram& program,
    const UniformVariable& vars, const TextureUnit& textureUnit) const;
   void activateGeometry() const;

   static MaterialUnit extractMaterialUnit(const aiMaterial* material);

   const aiMesh* mesh;
   const aiMaterial* material;
   MaterialUnit materialUnit;
   GeometryUnit geometryUnit;
   TextureGroupUnit textureGroupUnit;
};

} // namespace render
} // namespace crash
