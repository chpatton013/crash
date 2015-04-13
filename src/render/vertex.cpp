#include <crash/render/shader_program.hpp>
#include <crash/render/vertex.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

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

/* static */ void Vertex::defineAttributes() {
   Vertex::defineAttribute(Vertex::attributeDefinition.position);
   Vertex::defineAttribute(Vertex::attributeDefinition.normal);
   Vertex::defineAttribute(Vertex::attributeDefinition.tangent);
   Vertex::defineAttribute(Vertex::attributeDefinition.bitangent);
   Vertex::defineAttribute(Vertex::attributeDefinition.texture_coordinates);
}

/* static */ void Vertex::defineAttribute(Attribute attribute) {
   glVertexAttribPointer(attribute.index, attribute.width,
    GL_FLOAT, GL_FALSE, sizeof(Vertex),
    reinterpret_cast< const GLvoid* >(attribute.offset));
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
}

/* static */ void Vertex::bindAttribute(const ShaderProgram& program,
 const std::string& var, Attribute attribute) {
   glBindAttribLocation(program.getHandle(), attribute.index,
    var.data());
}

Vertex::Attribute::Attribute(
 unsigned int index, size_t offset, size_t width) :
   index(index), offset(offset), width(width)
{}

Vertex::AttributeDefinition::AttributeDefinition(
 Attribute position, Attribute normal,
 Attribute tangent, Attribute bitangent,
 Attribute texture_coordinates) :
 position(position), normal(normal),
 tangent(tangent), bitangent(bitangent),
 texture_coordinates(texture_coordinates)
{}

/* static */ Vertex::AttributeDefinition Vertex::attributeDefinition(
 Attribute(0, offsetof(Vertex, position), 3),
 Attribute(1, offsetof(Vertex, normal), 3),
 Attribute(2, offsetof(Vertex, tangent), 3),
 Attribute(3, offsetof(Vertex, bitangent), 3),
 Attribute(4, offsetof(Vertex, textureCoordinates), 2));
