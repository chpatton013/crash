#pragma once

#include <array>
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

namespace crash {
namespace render {

class ShaderProgram;
struct AttributeVariable;

struct Vertex {
   struct Attribute {
      Attribute(unsigned int index, GLenum type, size_t offset, size_t width);

      unsigned int index;
      GLenum type;
      size_t offset;
      size_t width;
   };

   Vertex(const Vertex& v);
   Vertex(const glm::vec3& position, const glm::vec3& normal,
    const glm::vec3& tangent, const glm::vec3& bitangent,
    const glm::vec2& textureCoordinates,
    const glm::ivec4& boneIds, const glm::vec4& boneWeights);

   static void defineAttributes();
   static void defineAttribute(Attribute attribute);

   static void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars);
   static void bindAttribute(const ShaderProgram& program,
    const std::string& var, Attribute attribute);

   glm::vec3 position;
   glm::vec3 normal;
   glm::vec3 tangent;
   glm::vec3 bitangent;
   glm::vec2 textureCoordinates;
   glm::ivec4 boneIds;
   glm::vec4 boneWeights;

   static struct AttributeDefinition {
      AttributeDefinition(Attribute position, Attribute normal,
       Attribute tangent, Attribute bitangent,
       Attribute texture_coordinates,
       Attribute bone_ids, Attribute bone_weights);

      Attribute position;
      Attribute normal;
      Attribute tangent;
      Attribute bitangent;
      Attribute texture_coordinates;
      Attribute bone_ids;
      Attribute bone_weights;
   } attributeDefinition;
};

} // namespace render
} // namespace crash
