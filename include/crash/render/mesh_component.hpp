#pragma once

#include <map>
#include <memory>
#include <tuple>
#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <crash/render/vertex.hpp>

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
   TextureGroupUnit(
    const TextureUnit& displacement,
    const TextureUnit& normal,
    const TextureUnit& ambient,
    const TextureUnit& diffuse,
    const TextureUnit& specular,
    const TextureUnit& shininess);
   TextureUnit displacement;
   TextureUnit normal;
   TextureUnit ambient;
   TextureUnit diffuse;
   TextureUnit specular;
   TextureUnit shininess;
};

struct BoneWeightUnit {
   BoneWeightUnit(unsigned int index, float weight);
   unsigned int index;
   float weight;
};

typedef std::vector< BoneWeightUnit > BoneWeightGroupUnit;

struct BoneTransformUnit {
   BoneTransformUnit(const glm::mat4& offset, const glm::mat4& transform);
   glm::mat4 offset;
   glm::mat4 transform;
};

typedef std::map< const aiBone*, const aiNode* > BoneNodeMap;
typedef std::map< const aiNode*, glm::mat4 > NodeTransformMap;

class MeshComponent {
public:
   static const glm::vec4 DEFAULT_AMBIENT_COLOR;
   static const glm::vec4 DEFAULT_DIFFUSE_COLOR;
   static const glm::vec4 DEFAULT_SPECULAR_COLOR;
   static const float DEFAULT_SHININESS;

   MeshComponent(const MeshComponent& component);
   MeshComponent(const aiNode* node, const aiMesh* mesh,
    const aiMaterial* material, const GeometryUnit& geometryUnit,
    const TextureGroupUnit& textureGroupUnit);

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;

   void render(const ShaderProgram& program, const UniformVariable& vars,
    const glm::mat4& modelTransform, const BoneNodeMap& boneNodes,
    const NodeTransformMap& nodeTransforms) const;

private:
   /////////////////////////////////////////////////////////////////////////////
   // Initialization.
   /////////////////////////////////////////////////////////////////////////////

   void fillVertexBones();
   void generateVertexArray() const;
   void generateVertexBuffer() const;
   void generateIndexBuffer() const;
   void generateTextureBuffers() const;

   /////////////////////////////////////////////////////////////////////////////
   // Rendering.
   /////////////////////////////////////////////////////////////////////////////

   void activateBones(const ShaderProgram& program,
    const UniformVariable& vars,
    const BoneNodeMap& boneNodes,
    const NodeTransformMap& nodeTransforms) const;
   void activateMaterial(const ShaderProgram& program,
    const UniformVariable& vars) const;
   void activateTextures(const ShaderProgram& program,
    const UniformVariable& vars) const;
   void activateGeometry() const;

   /////////////////////////////////////////////////////////////////////////////
   // Helpers.
   /////////////////////////////////////////////////////////////////////////////

   glm::vec3 getVertexPosition(unsigned int index) const;
   glm::vec3 getVertexNormal(unsigned int index) const;
   std::tuple< glm::vec3, glm::vec3 >
    getVertexTangentAndBitangent(unsigned int index) const;
   glm::vec2 getVertexTextureCoordinates(unsigned int index) const;
   std::tuple< glm::ivec4, glm::vec4 >
    getVertexBones(unsigned int index) const;

   void generateTextureBuffer(const TextureUnit& textureUnit) const;

   void activateTexture(const ShaderProgram& program,
    const std::string& hasTextureVar, const std::string& textureVar,
    const TextureUnit& textureUnit) const;

   static MaterialUnit extractMaterialUnit(const aiMaterial* material);

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   const aiNode* _node;
   const aiMesh* _mesh;
   const aiMaterial* _material;
   MaterialUnit _materialUnit;
   GeometryUnit _geometryUnit;
   TextureGroupUnit _textureGroupUnit;
   std::vector< BoneWeightGroupUnit > _vertexBones;
};

} // namespace render
} // namespace crash
