#include <glm/gtc/type_ptr.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/mesh_component.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/vertex.hpp>

using namespace crash::render;

/* static */ const glm::vec4 MeshComponent::defaultAmbientColor =
 glm::vec4(glm::vec3(0.1f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultDiffuseColor =
 glm::vec4(glm::vec3(0.7f), 1.0f);
/* static */ const glm::vec4 MeshComponent::defaultSpecularColor =
 glm::vec4(glm::vec3(0.9f), 1.0f);
/* static */ const float MeshComponent::defaultShininess = 250.0f;

MeshComponent::MeshComponent(const MeshComponent& component) :
   mesh(component.mesh), material(component.material),
    ambient(component.ambient), diffuse(component.diffuse),
    specular(component.specular), shininess(component.shininess),
    twoSided(component.twoSided), texture(component.texture),
    vao(component.vao), vbo(component.vbo), ibo(component.ibo),
    tbo(component.tbo)
{}

MeshComponent::MeshComponent(const aiMesh* mesh, const aiMaterial* material,
 std::shared_ptr< Texture > texture, const GLuint& vao, const GLuint& vbo,
 const GLuint& ibo, const GLuint& tbo) :
   mesh(mesh), material(material),
    ambient(MeshComponent::defaultAmbientColor),
    diffuse(MeshComponent::defaultDiffuseColor),
    specular(MeshComponent::defaultSpecularColor),
    shininess(MeshComponent::defaultShininess),
    twoSided(false), texture(texture),
    vao(vao), vbo(vbo), ibo(ibo), tbo(tbo)
{
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
   this->generateTextureBuffer();
   this->setMaterialProperties();
}

void MeshComponent::generateVertexArray() {
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBindVertexArray(this->vao);

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

   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
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

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(),
    faces.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateTextureBuffer() {
   GLint format;
   int components = this->texture->getComponents();
   if (components == 1) {
      format = GL_LUMINANCE;
   } else if (components == 2) {
      format = GL_LUMINANCE_ALPHA;
   } else if (components == 3) {
      format = GL_RGB;
   } else { // components == 4
      format = GL_RGBA;
   }

   glBindTexture(GL_TEXTURE_2D, this->tbo);
   glTexImage2D(GL_TEXTURE_2D,
    /* level of detail */ 0, /* texture format */ format,
    this->texture->getWidth(), this->texture->getHeight(),
    /* border */ 0, /* data format */ format, /* data type */ GL_FLOAT,
    this->texture->getData().data());
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
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindVertexArray(this->vao);

   Vertex::bindAttributes(program, vars);
}

void MeshComponent::render(const ShaderProgram& program,
 const UniformVariable& vars, const glm::mat4& transform) const {
   program.setUniformVariableMatrix4(vars.transform_matrix,
    glm::value_ptr(transform), 1);

   program.setUniformVariable4f(vars.ambient_color,
    glm::value_ptr(this->ambient), 1);
   program.setUniformVariable4f(vars.diffuse_color,
    glm::value_ptr(this->diffuse), 1);
   program.setUniformVariable4f(vars.specular_color,
    glm::value_ptr(this->specular), 1);
   program.setUniformVariable1f(vars.shininess_value, &this->shininess, 1);

   glBindVertexArray(this->vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

   GLuint hasTexture = (this->texture != nullptr) ? 1 : 0;
   program.setUniformVariable1ui(vars.has_diffuse_texture, &hasTexture, 1);
   if (hasTexture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->tbo);
      int activeTexture = 0;
      program.setUniformVariable1i(vars.diffuse_texture, &activeTexture, 1);
   }

   if (this->twoSided) {
      glDisable(GL_CULL_FACE);
   } else {
      glEnable(GL_CULL_FACE);
   }

   glDrawElements(GL_TRIANGLES, this->mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
}

void MeshComponent::setMaterialProperties() {
   if (this->material == nullptr) {
      return;
   }

   aiColor4D aiAmbient(0.0f, 0.0f, 0.0f, 1.0f);
   this->material->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);
   if (!aiAmbient.IsBlack()) {
      this->ambient = glm::vec4(aiAmbient.r, aiAmbient.g, aiAmbient.g,
       aiAmbient.a);
   }

   aiColor4D aiDiffuse(0.0f, 0.0f, 0.0f, 1.0f);
   this->material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
   if (!aiDiffuse.IsBlack()) {
      this->diffuse = glm::vec4(aiDiffuse.r, aiDiffuse.g, aiDiffuse.g,
       aiDiffuse.a);
   }

   aiColor4D aiSpecular(0.0f, 0.0f, 0.0f, 1.0f);
   this->material->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);
   if (!aiSpecular.IsBlack()) {
      this->specular = glm::vec4(aiSpecular.r, aiSpecular.g, aiSpecular.g,
       aiSpecular.a);
   }

   float shininess = 0.0f;
   this->material->Get(AI_MATKEY_SHININESS, shininess);
   if (shininess != 0.0f) {
      this->shininess = shininess;
   }

   int iTwoSided;
   this->material->Get(AI_MATKEY_TWOSIDED, iTwoSided);
   this->twoSided = (iTwoSided != 0);
}
