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
 unsigned int index) :
   texture(texture), tbo(tbo), index(index)
{}

/* static */ const glm::vec4 MeshComponent::defaultAmbientColor =
 glm::vec4(glm::vec3(0.1f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultDiffuseColor =
 glm::vec4(glm::vec3(0.7f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultSpecularColor =
 glm::vec4(glm::vec3(0.9f), 1.0f);
/* static */ const float MeshComponent::defaultShininess = 250.0f;

MeshComponent::MeshComponent(const MeshComponent& component) :
   mesh(component.mesh), material(component.material),
    materialUnit(component.materialUnit),
    geometryUnit(component.geometryUnit),
    textureUnit(component.textureUnit)
{}

MeshComponent::MeshComponent(const aiMesh* mesh, const aiMaterial* material,
 const GeometryUnit& geometryUnit, const TextureUnit& textureUnit) :
   mesh(mesh), material(material),
    materialUnit(MeshComponent::extractMaterialUnit(material)),
    geometryUnit(geometryUnit), textureUnit(textureUnit)
{
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
   this->generateTextureBuffer();
}

void MeshComponent::generateVertexArray() {
   glBindBuffer(GL_ARRAY_BUFFER, this->geometryUnit.vbo);
   glBindVertexArray(this->geometryUnit.vao);

   Vertex::defineAttributes();
}

void MeshComponent::generateVertexBuffer() {
   std::vector< Vertex > vertices;

   const aiVector3D zero(0.0f, 0.0f, 0.0f);

   for (unsigned int i = 0; i < this->mesh->mNumVertices; ++i) {
      auto position = this->mesh->mVertices[i];

      auto normal = zero;
      if (this->mesh->HasNormals()) {
         normal = this->mesh->mNormals[i];
      }

      auto tangent = zero;
      auto bitangent = zero;
      if (this->mesh->HasTangentsAndBitangents()) {
         tangent = this->mesh->mTangents[i];
         bitangent = this->mesh->mBitangents[i];
      }

      auto textureCoordinates = aiVector2D(0.0f, 0.0f);
      if (this->mesh->HasTextureCoords(0)) {
         auto texture = this->mesh->mTextureCoords[0][i];
         textureCoordinates.x = texture.x;
         textureCoordinates.y = texture.y;
      }

      vertices.push_back(Vertex(position, normal, tangent, bitangent,
       textureCoordinates));
   }

   glBindBuffer(GL_ARRAY_BUFFER, this->geometryUnit.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
    vertices.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateIndexBuffer() {
   std::vector< GLuint > faces;
   faces.resize(this->mesh->mNumFaces * 3);

   for (unsigned int i = 0; i < this->mesh->mNumFaces; i++) {
      const aiFace& face = this->mesh->mFaces[i];
      for (unsigned int j = 0; j < 3; ++j) {
         faces[i * 3 + j] = face.mIndices[j];
      }
   }

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->geometryUnit.ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(),
    faces.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateTextureBuffer() {
   GLint format;
   int components = this->textureUnit.texture->getComponents();
   if (components == 1) {
      format = GL_LUMINANCE;
   } else if (components == 2) {
      format = GL_LUMINANCE_ALPHA;
   } else if (components == 3) {
      format = GL_RGB;
   } else { // components == 4
      format = GL_RGBA;
   }

   glBindTexture(GL_TEXTURE_2D, this->textureUnit.tbo);
   glTexImage2D(GL_TEXTURE_2D,
    /* level of detail */ 0, /* texture format */ format,
    this->textureUnit.texture->getWidth(), this->textureUnit.texture->getHeight(),
    /* border */ 0, /* data format */ format, /* data type */ GL_FLOAT,
    this->textureUnit.texture->getData().data());
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
   glBindBuffer(GL_ARRAY_BUFFER, this->geometryUnit.vbo);
   glBindVertexArray(this->geometryUnit.vao);

   Vertex::bindAttributes(program, vars);
}

void MeshComponent::render(const ShaderProgram& program,
 const UniformVariable& vars, const glm::mat4& transform) const {
   program.setUniformVariableMatrix4(vars.transform_matrix,
    glm::value_ptr(transform), 1);

   program.setUniformVariable4f(vars.ambient_color,
    glm::value_ptr(this->materialUnit.ambient), 1);
   program.setUniformVariable4f(vars.diffuse_color,
    glm::value_ptr(this->materialUnit.diffuse), 1);
   program.setUniformVariable4f(vars.specular_color,
    glm::value_ptr(this->materialUnit.specular), 1);
   program.setUniformVariable1f(vars.shininess_value,
    &this->materialUnit.shininess, 1);

   glBindVertexArray(this->geometryUnit.vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->geometryUnit.vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->geometryUnit.ibo);

   GLuint hasTexture = (this->textureUnit.texture != nullptr) ? 1 : 0;
   program.setUniformVariable1ui(vars.has_diffuse_texture, &hasTexture, 1);
   if (hasTexture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->textureUnit.tbo);
      int activeTexture = 0;
      program.setUniformVariable1i(vars.diffuse_texture, &activeTexture, 1);
   }

   if (this->materialUnit.twoSided) {
      glDisable(GL_CULL_FACE);
   } else {
      glEnable(GL_CULL_FACE);
   }

   glDrawElements(GL_TRIANGLES, this->mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
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
