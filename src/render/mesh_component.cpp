#include <glm/gtc/type_ptr.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/mesh_component.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/vertex.hpp>

using namespace crash::render;

MaterialUnit::MaterialUnit(const glm::vec4& ambient, const glm::vec4& diffuse,
 const glm::vec4& specular, float shininess, bool twoSided) :
   ambient(ambient), diffuse(diffuse), specular(specular),
   shininess(shininess), twoSided(twoSided)
{}

GeometryUnit::GeometryUnit(const GLuint& vao, const GLuint& vbo,
 const GLuint& ibo) :
   vao(vao), vbo(vbo), ibo(ibo)
{}

TextureUnit::TextureUnit(std::shared_ptr< Texture > texture, const GLuint& tbo,
 const GLint& index) :
   texture(texture), tbo(tbo), index(index)
{}

TextureGroupUnit::TextureGroupUnit(const TextureUnit& diffuse) :
   diffuse(diffuse)
{}

/* static */ const glm::vec4 MeshComponent::defaultAmbientColor =
 glm::vec4(glm::vec3(0.1f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultDiffuseColor =
 glm::vec4(glm::vec3(0.7f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultSpecularColor =
 glm::vec4(glm::vec3(0.9f), 1.0f);
/* static */ const float MeshComponent::defaultShininess = 250.0f;

MeshComponent::MeshComponent(const MeshComponent& component) :
   _mesh(component._mesh), _material(component._material),
    _materialUnit(component._materialUnit),
    _geometryUnit(component._geometryUnit),
    _textureGroupUnit(component._textureGroupUnit)
{}

MeshComponent::MeshComponent(const aiMesh* mesh, const aiMaterial* material,
 const GeometryUnit& geometryUnit, const TextureGroupUnit& textureGroupUnit) :
   _mesh(mesh), _material(material),
    _materialUnit(MeshComponent::extractMaterialUnit(material)),
    _geometryUnit(geometryUnit), _textureGroupUnit(textureGroupUnit)
{
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
   this->generateTextureBuffers();
}

void MeshComponent::generateVertexArray() {
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindVertexArray(this->_geometryUnit.vao);

   Vertex::defineAttributes();
}

void MeshComponent::generateVertexBuffer() {
   std::vector< Vertex > vertices;

   const aiVector3D zero(0.0f, 0.0f, 0.0f);

   for (unsigned int i = 0; i < this->_mesh->mNumVertices; ++i) {
      auto position = this->_mesh->mVertices[i];

      auto normal = zero;
      if (this->_mesh->HasNormals()) {
         normal = this->_mesh->mNormals[i];
      }

      auto tangent = zero;
      auto bitangent = zero;
      if (this->_mesh->HasTangentsAndBitangents()) {
         tangent = this->_mesh->mTangents[i];
         bitangent = this->_mesh->mBitangents[i];
      }

      auto textureCoordinates = aiVector2D(0.0f, 0.0f);
      if (this->_mesh->HasTextureCoords(0)) {
         auto texture = this->_mesh->mTextureCoords[0][i];
         textureCoordinates.x = texture.x;
         textureCoordinates.y = texture.y;
      }

      vertices.push_back(Vertex(position, normal, tangent, bitangent,
       textureCoordinates));
   }

   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
    vertices.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateIndexBuffer() {
   std::vector< GLuint > faces;
   faces.resize(this->_mesh->mNumFaces * 3);

   for (unsigned int i = 0; i < this->_mesh->mNumFaces; i++) {
      const aiFace& face = this->_mesh->mFaces[i];
      for (unsigned int j = 0; j < 3; ++j) {
         faces[i * 3 + j] = face.mIndices[j];
      }
   }

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_geometryUnit.ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(),
    faces.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateTextureBuffers() {
   this->generateTextureBuffer(this->_textureGroupUnit.diffuse);
}

void MeshComponent::generateTextureBuffer(const TextureUnit& textureUnit) {
   GLint format;
   int components = textureUnit.texture->getComponents();
   if (components == 1) {
      format = GL_LUMINANCE;
   } else if (components == 2) {
      format = GL_LUMINANCE_ALPHA;
   } else if (components == 3) {
      format = GL_RGB;
   } else { // components == 4
      format = GL_RGBA;
   }

   glBindTexture(GL_TEXTURE_2D, textureUnit.tbo);
   glTexImage2D(GL_TEXTURE_2D,
    /* level of detail */ 0, /* texture format */ format,
    textureUnit.texture->getWidth(), textureUnit.texture->getHeight(),
    /* border */ 0, /* data format */ format, /* data type */ GL_FLOAT,
    textureUnit.texture->getData().data());
   glGenerateMipmap(GL_TEXTURE_2D);

   // Repeat with extra space in x-direction.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   // Repeat with extra space in y-direction.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // Use mipmaps when down-sampling.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);
   // Use mipmaps when up-sampling.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);
}

void MeshComponent::bindAttributes(const ShaderProgram& program,
 const AttributeVariable& vars) const {
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindVertexArray(this->_geometryUnit.vao);

   Vertex::bindAttributes(program, vars);
}

void MeshComponent::render(const ShaderProgram& program,
 const UniformVariable& vars, const glm::mat4& transform) const {
   program.setUniformVariableMatrix4(vars.transform_matrix,
    glm::value_ptr(transform), 1);

   this->activateMaterial(program, vars);
   this->activateTextures(program, vars);
   this->activateGeometry();

   glDrawElements(GL_TRIANGLES, this->_mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
}

void MeshComponent::activateMaterial(const ShaderProgram& program,
 const UniformVariable& vars) const {
   program.setUniformVariable4f(vars.ambient_color,
    glm::value_ptr(this->_materialUnit.ambient), 1);
   program.setUniformVariable4f(vars.diffuse_color,
    glm::value_ptr(this->_materialUnit.diffuse), 1);
   program.setUniformVariable4f(vars.specular_color,
    glm::value_ptr(this->_materialUnit.specular), 1);
   program.setUniformVariable1f(vars.shininess_value,
    &this->_materialUnit.shininess, 1);

   if (this->_materialUnit.twoSided) {
      glDisable(GL_CULL_FACE);
   } else {
      glEnable(GL_CULL_FACE);
   }
}

void MeshComponent::activateTextures(const ShaderProgram& program,
 const UniformVariable& vars) const {
   this->activateTexture(program, vars, this->_textureGroupUnit.diffuse);
}

void MeshComponent::activateTexture(const ShaderProgram& program,
 const UniformVariable& vars, const TextureUnit& textureUnit) const {
   if (textureUnit.texture == nullptr) {
      GLuint hasTexture = 0;
      program.setUniformVariable1ui(vars.has_diffuse_texture, &hasTexture, 1);
   } else {
      GLuint hasTexture = 1;
      program.setUniformVariable1ui(vars.has_diffuse_texture, &hasTexture, 1);

      glActiveTexture(GL_TEXTURE0 + textureUnit.index);
      glBindTexture(GL_TEXTURE_2D, textureUnit.tbo);
      program.setUniformVariable1i(vars.diffuse_texture, &textureUnit.index, 1);
   }
}

void MeshComponent::activateGeometry() const {
   glBindVertexArray(this->_geometryUnit.vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_geometryUnit.ibo);
}

/* static */ MaterialUnit MeshComponent::extractMaterialUnit(
 const aiMaterial* material) {
   glm::vec4 ambient = MeshComponent::defaultAmbientColor;
   glm::vec4 diffuse = MeshComponent::defaultDiffuseColor;
   glm::vec4 specular = MeshComponent::defaultSpecularColor;
   float shininess = MeshComponent::defaultShininess;
   bool twoSided = false;

   if (material == nullptr) {
      return MaterialUnit(ambient, diffuse, specular, shininess, twoSided);
   }

   aiColor4D aiAmbient(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);
   if (!aiAmbient.IsBlack()) {
      ambient = glm::vec4(aiAmbient.r, aiAmbient.g, aiAmbient.g, aiAmbient.a);
   }

   aiColor4D aiDiffuse(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
   if (!aiDiffuse.IsBlack()) {
      diffuse = glm::vec4(aiDiffuse.r, aiDiffuse.g, aiDiffuse.g, aiDiffuse.a);
   }

   aiColor4D aiSpecular(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);
   if (!aiSpecular.IsBlack()) {
      specular = glm::vec4(aiSpecular.r, aiSpecular.g, aiSpecular.g,
       aiSpecular.a);
   }

   float aiShininess = 0.0f;
   material->Get(AI_MATKEY_SHININESS, aiShininess);
   if (aiShininess != 0.0f) {
      shininess = aiShininess;
   }

   int aiTwoSided;
   material->Get(AI_MATKEY_TWOSIDED, aiTwoSided);
   twoSided = (aiTwoSided != 0);

   return MaterialUnit(ambient, diffuse, specular, shininess, twoSided);
}
