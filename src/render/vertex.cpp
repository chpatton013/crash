#include <crash/render/shader_program.hpp>
#include <crash/render/vertex.hpp>

using namespace crash::render;

Vertex::Vertex(const Vertex& v) :
   position(v.position), normal(v.normal),
    tangent(v.tangent), bitangent(v.bitangent),
    textureCoordinates(v.textureCoordinates)
{}

Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal,
 const glm::vec3& tangent, const glm::vec3& bitangent,
 const glm::vec2& textureCoordinates,
 const glm::ivec4& boneIds, const glm::vec4& boneWeights) :
   position(position), normal(normal),
    tangent(tangent), bitangent(bitangent),
    textureCoordinates(textureCoordinates),
    boneIds(boneIds), boneWeights(boneWeights)
{}

/* static */ void Vertex::defineAttributes() {
   Vertex::defineAttribute(Vertex::attributeDefinition.position);
   Vertex::defineAttribute(Vertex::attributeDefinition.normal);
   Vertex::defineAttribute(Vertex::attributeDefinition.tangent);
   Vertex::defineAttribute(Vertex::attributeDefinition.bitangent);
   Vertex::defineAttribute(Vertex::attributeDefinition.texture_coordinates);
   Vertex::defineAttribute(Vertex::attributeDefinition.bone_ids);
   Vertex::defineAttribute(Vertex::attributeDefinition.bone_weights);
}

/* static */ void Vertex::defineAttribute(Attribute attribute) {
   if (attribute.type == GL_HALF_FLOAT || attribute.type == GL_FLOAT ||
    attribute.type == GL_DOUBLE || attribute.type == GL_FIXED) {
      glVertexAttribPointer(attribute.index, attribute.width,
       attribute.type, GL_FALSE, sizeof(Vertex),
       reinterpret_cast< const GLvoid* >(attribute.offset));
   } else { // All integral types
      glVertexAttribIPointer(attribute.index, attribute.width,
       attribute.type, sizeof(Vertex),
       reinterpret_cast< const GLvoid* >(attribute.offset));
   }
   glEnableVertexAttribArray(attribute.index);
}

/* static */ void Vertex::bindAttributes(const ShaderProgram& program,
 const AttributeVariable& vars) {
   Vertex::bindAttribute(program, vars.position,
    Vertex::attributeDefinition.position);
   Vertex::bindAttribute(program, vars.normal,
    Vertex::attributeDefinition.normal);
   Vertex::bindAttribute(program, vars.tangent,
    Vertex::attributeDefinition.tangent);
   Vertex::bindAttribute(program, vars.bitangent,
    Vertex::attributeDefinition.bitangent);
   Vertex::bindAttribute(program, vars.texture_coordinates,
    Vertex::attributeDefinition.texture_coordinates);
   Vertex::bindAttribute(program, vars.bone_ids,
    Vertex::attributeDefinition.bone_ids);
   Vertex::bindAttribute(program, vars.bone_weights,
    Vertex::attributeDefinition.bone_weights);
}

/* static */ void Vertex::bindAttribute(const ShaderProgram& program,
 const std::string& var, Attribute attribute) {
   glBindAttribLocation(program.getHandle(), attribute.index,
    var.data());
}

Vertex::Attribute::Attribute(
 unsigned int index, GLenum type, size_t offset, size_t width) :
   index(index), type(type), offset(offset), width(width)
{}

Vertex::AttributeDefinition::AttributeDefinition(
 const Attribute& position, const Attribute& normal,
 const Attribute& tangent, const Attribute& bitangent,
 const Attribute& texture_coordinates,
 const Attribute& bone_ids, const Attribute& bone_weights) :
   position(position), normal(normal),
    tangent(tangent), bitangent(bitangent),
    texture_coordinates(texture_coordinates),
    bone_ids(bone_ids), bone_weights(bone_weights)
{}

/* static */ Vertex::AttributeDefinition Vertex::attributeDefinition(
   Attribute(0, GL_FLOAT, offsetof(Vertex, position), 3),
   Attribute(1, GL_FLOAT, offsetof(Vertex, normal), 3),
   Attribute(2, GL_FLOAT, offsetof(Vertex, tangent), 3),
   Attribute(3, GL_FLOAT, offsetof(Vertex, bitangent), 3),
   Attribute(4, GL_FLOAT, offsetof(Vertex, textureCoordinates), 2),
   Attribute(5, GL_INT, offsetof(Vertex, boneIds), 4),
   Attribute(6, GL_FLOAT, offsetof(Vertex, boneWeights), 4)
);
