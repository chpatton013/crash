#pragma once

#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>

namespace crash {
namespace render {

struct Vertex {
   Vertex(const Vertex& v);
   Vertex(const glm::vec3& position, const glm::vec3& normal,
    const glm::vec3& tangent, const glm::vec3& bitangent,
    const glm::vec2& textureCoordinates);
   Vertex(const aiVector3D& position, const aiVector3D& normal,
    const aiVector3D& tangent, const aiVector3D& bitangent,
    const aiVector2D& textureCoordinates);

   glm::vec3 position;
   glm::vec3 normal;
   glm::vec3 tangent;
   glm::vec3 bitangent;
   glm::vec2 textureCoordinates;

   struct Attribute {
      Attribute(unsigned int index, const std::string& name,
       size_t offset, size_t width);

      unsigned int index;
      std::string name;
      size_t offset;
      size_t width;
   };

   static std::vector< Attribute > attributes;
};

} // namespace render
} // namespace crash
