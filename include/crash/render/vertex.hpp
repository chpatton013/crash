#pragma once

#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>

namespace crash {
namespace render {

class ShaderProgram;
struct AttributeVariable;

struct Vertex {
   struct Attribute {
      Attribute(unsigned int index, size_t offset, size_t width);

      unsigned int index;
      size_t offset;
      size_t width;
   };

   Vertex(const Vertex& v);
   Vertex(const glm::vec3& position, const glm::vec3& normal,
    const glm::vec3& tangent, const glm::vec3& bitangent,
    const glm::vec2& textureCoordinates);
   Vertex(const aiVector3D& position, const aiVector3D& normal,
    const aiVector3D& tangent, const aiVector3D& bitangent,
    const aiVector2D& textureCoordinates);

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

   struct AttributeDefinition {
      AttributeDefinition(Attribute position, Attribute normal,
       Attribute tangent, Attribute bitangent,
       Attribute texture_coordinates);

      Attribute position;
      Attribute normal;
      Attribute tangent;
      Attribute bitangent;
      Attribute texture_coordinates;
   };

   static AttributeDefinition attributeDefinition;
};

} // namespace render
} // namespace crash
