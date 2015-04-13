#include <crash/render/vertex.hpp>

using namespace crash::render;

Vertex::Vertex(const Vertex& v) :
   position(v.position), normal(v.normal),
    tangent(v.tangent), bitangent(v.bitangent),
    textureCoordinates(v.textureCoordinates)
{}

Vertex::Vertex(const glm::vec3& position,
 const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent,
 const glm::vec2& textureCoordinates) :
   position(position), normal(normal),
    tangent(tangent), bitangent(bitangent),
    textureCoordinates(textureCoordinates)
{}

Vertex::Vertex(const aiVector3D& position,
 const aiVector3D& normal, const aiVector3D& tangent,
 const aiVector3D& bitangent, const aiVector2D& textureCoordinates) :
   Vertex(glm::vec3(position.x, position.y, position.z),
    glm::vec3(normal.x, normal.y, normal.z),
    glm::vec3(tangent.x, tangent.y, tangent.z),
    glm::vec3(bitangent.x, bitangent.y, bitangent.z),
    glm::vec2(textureCoordinates.x, textureCoordinates.y))
{}

Vertex::Attribute::Attribute(
 unsigned int index, const std::string& name, size_t offset, size_t width) :
   index(index), name(name), offset(offset), width(width)
{}

/* static */ std::vector< Vertex::Attribute > Vertex::attributes = {{
   Attribute(0, "aPosition", offsetof(Vertex, position), 3),
   Attribute(1, "aNormal", offsetof(Vertex, normal), 3),
   Attribute(2, "aTangent", offsetof(Vertex, tangent), 3),
   Attribute(3, "aBitangent", offsetof(Vertex, bitangent), 3),
   Attribute(4, "aTexCoords", offsetof(Vertex, textureCoordinates), 2),
}};

