#include <vector>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <crash/math/arithmetic.hpp>
#include <crash/math/symbols.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader_program.hpp>

using namespace crash::math;
using namespace crash::render;

/* static */ const glm::vec4 Mesh::defaultAmbientColor =
 glm::vec4(glm::vec3(0.1f), 1.0f);
/* static */ const glm::vec4 Mesh::defaultDiffuseColor =
 glm::vec4(glm::vec3(0.7f), 1.0f);
/* static */ const glm::vec4 Mesh::defaultSpecularColor =
 glm::vec4(glm::vec3(0.9f), 1.0f);
/* static */ const float Mesh::defaultShininess = 250.0f;

Mesh::SceneImportFailure::SceneImportFailure(const std::string& error) :
   error(error)
{}

Mesh::VariableSignature::VariableSignature(const std::string& transform,
  const std::string& ambient, const std::string& diffuse,
  const std::string& specular, const std::string& shininess,
  const std::string& hasTexture, const std::string& texture) :
   transform(transform), ambient(ambient), diffuse(diffuse),
    specular(specular), shininess(shininess),
    hasTexture(hasTexture), texture(texture)
{}

////////////////////////////////////////////////////////////////////////////////
// Mesh
////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const Mesh& mesh) :
   _path(mesh._path), _scene(mesh._scene), _transformer(mesh._transformer),
    _vaos(mesh._vaos), _vbos(mesh._vbos), _ibos(mesh._ibos), _tbos(mesh._tbos),
    _components(mesh._components), _textures(mesh._textures)
{}

Mesh::Mesh(const boost::filesystem::path& path) :
   _path(path), _scene(nullptr),
    _transformer(glm::vec3(), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)),
    _vaos(), _vbos(), _ibos(), _tbos(), _components()
{
   this->importScene();
   this->importTextures();
}

/* virtual */ Mesh::~Mesh() {
   this->releaseScene();
}

////////////////////////////////////////////////////////////////////////////////
// Transformable interface.
////////////////////////////////////////////////////////////////////////////////

const glm::vec3& Mesh::getPosition() const {
   return this->_transformer.getPosition();
}

const glm::vec4& Mesh::getOrientation() const {
   return this->_transformer.getOrientation();
}

const glm::vec3& Mesh::getSize() const {
   return this->_transformer.getSize();
}

void Mesh::setPosition(const glm::vec3& position) {
   this->_transformer.setPosition(position);
}

void Mesh::setOrientation(const glm::vec4& orientation) {
   this->_transformer.setOrientation(orientation);
}

void Mesh::setSize(const glm::vec3& size) {
   this->_transformer.setSize(size);
}

const glm::mat4& Mesh::getTransform() {
   return this->_transformer.getTransform();
}

////////////////////////////////////////////////////////////////////////////////
// Rendering.
////////////////////////////////////////////////////////////////////////////////

void Mesh::initialize() {
   this->allocateBuffers();
   this->buildComponents();
}

void Mesh::teardown() {
   this->destroyComponents();
   this->releaseBuffers();
}

void Mesh::bindAttributes(const ShaderProgram& program) const {
   for (auto itr : this->_components) {
      itr.second.bindAttributes(program);
   }
}

void Mesh::render(const ShaderProgram& program, const VariableSignature& sig,
 MatrixStack& matrixStack) {
   matrixStack.push(this->getTransform());
   this->renderNode(program, sig, matrixStack, this->_scene->mRootNode);
   matrixStack.pop();
}

/**
 * Post-processing flags:
 *    aiProcess_CalcTangentSpace
 *    aiProcess_FixInfacingNormals
 *    aiProcess_FindDegenerates
 *    aiProcess_FindInstances
 *    aiProcess_FindInvalidData
 *    aiProcess_GenSmoothNormals
 *    aiProcess_GenUVCoords
 *    aiProcess_ImproveCacheLocality
 *    aiProcess_JoinIdenticalVertices
 *    aiProcess_LimitBoneWeights
 *    aiProcess_OptimizeMeshes
 *    aiProcess_OptimizeGraph
 *    aiProcess_RemoveRedundantMaterials
 *    aiProcess_SortByPType
 *    aiProcess_SplitLargeMeshes
 *    aiProcess_TransformUVCoords
 *    aiProcess_Triangulate
 *    aiProcess_ValidateDataStructure;
 */
void Mesh::importScene() {
   static const unsigned int postProcessingFlags =
      aiProcessPreset_TargetRealtime_MaxQuality
    | aiProcess_FixInfacingNormals
    | aiProcess_OptimizeGraph
    | aiProcess_TransformUVCoords;

   this->releaseScene();

   auto scene = aiImportFile(this->_path.string().data(), postProcessingFlags);
   if (scene == nullptr) {
      throw SceneImportFailure(aiGetErrorString());
   }

   this->_scene = scene;
   this->normalizeScene();
}

void Mesh::importTextures() {
   for (unsigned int i = 0; i < this->_scene->mNumMaterials; ++i) {
      aiMaterial* mat = this->_scene->mMaterials[i];

      aiString textureFileName;
      mat->GetTexture(aiTextureType_DIFFUSE, /* index */ 0, &textureFileName,
       /* mapping */ nullptr, /* uv index */ nullptr, /* blend */ nullptr,
       /* operator */ nullptr, /* map mode */ nullptr);

      boost::filesystem::path texturePath =
       boost::filesystem::path(this->_path).parent_path() /
       boost::filesystem::path(textureFileName.C_Str());

      this->_textures.push_back(std::make_shared< Texture >(
       Texture(texturePath)));
   }
}

void Mesh::normalizeScene() {
   glm::vec3 min(std::numeric_limits< float >::max());
   glm::vec3 max(std::numeric_limits< float >::min());
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      aiMesh* mesh = this->_scene->mMeshes[i];
      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
         aiVector3D aiVertex = mesh->mVertices[j];
         glm::vec3 vertex(aiVertex.x, aiVertex.y, aiVertex.z);
         min = glm::min(min, vertex);
         max = glm::max(max, vertex);
      }
   }

   glm::vec3 dimensions = glm::abs(max - min);
   float maxDimension = glm::max(glm::max(dimensions.x, dimensions.y),
    dimensions.z);
   float scale = 1.0f / maxDimension;
   glm::vec3 center = average(min, max) * scale;

   this->setPosition(-center);
   this->setSize(glm::vec3(scale));
}

void Mesh::releaseScene() {
   if (this->_scene == nullptr) {
      return;
   }

   aiReleaseImport(this->_scene);
   this->_scene = nullptr;
}

void Mesh::buildComponents() {
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      aiMesh* mesh = this->_scene->mMeshes[i];
      aiMaterial* material = this->_scene->mMaterials[mesh->mMaterialIndex];
      std::shared_ptr< Texture > texture;
      if (mesh->mMaterialIndex < this->_textures.size()) {
         texture = this->_textures[mesh->mMaterialIndex];
      } else {
         texture = nullptr;
      }

      GLuint vao = this->_vaos[i];
      GLuint vbo = this->_vbos[i];
      GLuint ibo = this->_ibos[i];
      GLuint tbo = this->_tbos[mesh->mMaterialIndex];

      this->_components.insert(std::make_pair(mesh,
       Component(mesh, material, texture, vao, vbo, ibo, tbo)));
   }
}

void Mesh::allocateBuffers() {
   unsigned int numBuffers = this->_scene->mNumMeshes;
   unsigned int numTextures = this->_scene->mNumMaterials;

   this->_vaos.clear();
   this->_vaos.resize(numBuffers);
   this->_vaos.insert(this->_vaos.end(), 0);
   glGenVertexArrays(numBuffers, this->_vaos.data());

   this->_vbos.clear();
   this->_vbos.resize(numBuffers);
   this->_vbos.insert(this->_vbos.end(), 0);
   glGenBuffers(numBuffers, this->_vbos.data());

   this->_ibos.clear();
   this->_ibos.resize(numBuffers);
   this->_ibos.insert(this->_ibos.end(), 0);
   glGenBuffers(numBuffers, this->_ibos.data());

   this->_tbos.clear();
   this->_tbos.resize(numTextures);
   this->_tbos.insert(this->_tbos.end(), 0);
   glGenTextures(numTextures, this->_tbos.data());
}

void Mesh::destroyComponents() {
   this->_components.clear();
}

void Mesh::releaseBuffers() {
   for (auto& attribute : Mesh::Component::Vertex::attributes) {
      glDisableVertexAttribArray(attribute.index);
   }

   glDeleteVertexArrays(this->_vaos.size(), this->_vaos.data());
   glDeleteBuffers(this->_vbos.size(), this->_vbos.data());
   glDeleteBuffers(this->_ibos.size(), this->_ibos.data());
   glDeleteTextures(this->_tbos.size(), this->_tbos.data());
}

void Mesh::renderNode(const ShaderProgram& program,
 const VariableSignature& sig, MatrixStack& matrixStack,
 const aiNode* node) const {
   auto m = node->mTransformation;
   glm::mat4 transform = glm::mat4(
      m.a1, m.a2, m.a3, m.a4,
      m.b1, m.b2, m.b3, m.b4,
      m.c1, m.c2, m.c3, m.c4,
      m.d1, m.d2, m.d3, m.d4
   );

   matrixStack.push(transform);

   for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
      auto mesh = this->_scene->mMeshes[node->mMeshes[i]];
      auto itr = this->_components.find(mesh);
      if (itr != this->_components.end()) {
         auto component = itr->second;
         component.render(program, sig, matrixStack.top());
      }
   }

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->renderNode(program, sig, matrixStack, node->mChildren[i]);
   }

   matrixStack.pop();
}

////////////////////////////////////////////////////////////////////////////////
// Mesh::Component
////////////////////////////////////////////////////////////////////////////////

Mesh::Component::Component(const Component& component) :
   mesh(component.mesh), material(component.material),
    ambient(component.ambient), diffuse(component.diffuse),
    specular(component.specular), shininess(component.shininess),
    twoSided(component.twoSided), texture(component.texture),
    vao(component.vao), vbo(component.vbo), ibo(component.ibo),
    tbo(component.tbo)
{}

Mesh::Component::Component(const aiMesh* mesh, const aiMaterial* material,
 std::shared_ptr< Texture > texture, const GLuint& vao, const GLuint& vbo,
 const GLuint& ibo, const GLuint& tbo) :
   mesh(mesh), material(material),
    ambient(Mesh::defaultAmbientColor), diffuse(Mesh::defaultDiffuseColor),
    specular(Mesh::defaultSpecularColor), shininess(Mesh::defaultShininess),
    twoSided(false), texture(texture),
    vao(vao), vbo(vbo), ibo(ibo), tbo(tbo)
{
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
   this->generateTextureBuffer();
   this->setMaterialProperties();
}

void Mesh::Component::generateVertexArray() {
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBindVertexArray(this->vao);

   for (auto& attribute : Mesh::Component::Vertex::attributes) {
      glVertexAttribPointer(attribute.index, attribute.width, GL_FLOAT,
       GL_FALSE, sizeof(Vertex),
       reinterpret_cast< const GLvoid* >(attribute.offset));
      glEnableVertexAttribArray(attribute.index);
   }
}

void Mesh::Component::generateVertexBuffer() {
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

void Mesh::Component::generateIndexBuffer() {
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

void Mesh::Component::generateTextureBuffer() {
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

void Mesh::Component::bindAttributes(const ShaderProgram& program) const {
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindVertexArray(this->vao);
   for (auto& attribute : Mesh::Component::Vertex::attributes) {
      glBindAttribLocation(program.getHandle(), attribute.index,
       attribute.name.data());
   }
}

void Mesh::Component::render(const ShaderProgram& program,
 const VariableSignature& sig, const glm::mat4& transform) const {
   program.setUniformVariableMatrix4(sig.transform, glm::value_ptr(transform), 1);

   program.setUniformVariable4f(sig.ambient, glm::value_ptr(this->ambient), 1);
   program.setUniformVariable4f(sig.diffuse, glm::value_ptr(this->diffuse), 1);
   program.setUniformVariable4f(sig.specular, glm::value_ptr(this->specular), 1);
   program.setUniformVariable1f(sig.shininess, &this->shininess, 1);

   glBindVertexArray(this->vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

   GLuint hasTexture = (this->texture != nullptr) ? 1 : 0;
   program.setUniformVariable1ui(sig.hasTexture, &hasTexture, 1);
   if (hasTexture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->tbo);
      int activeTexture = 0;
      program.setUniformVariable1i(sig.texture, &activeTexture, 1);
   }

   if (this->twoSided) {
      glDisable(GL_CULL_FACE);
   } else {
      glEnable(GL_CULL_FACE);
   }

   glDrawElements(GL_TRIANGLES, this->mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
}

void Mesh::Component::setMaterialProperties() {
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

////////////////////////////////////////////////////////////////////////////////
// Mesh::Component::Vertex
////////////////////////////////////////////////////////////////////////////////

Mesh::Component::Vertex::Vertex(const Vertex& v) :
   position(v.position), normal(v.normal),
    tangent(v.tangent), bitangent(v.bitangent),
    textureCoordinates(v.textureCoordinates)
{}

Mesh::Component::Vertex::Vertex(const glm::vec3& position,
 const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent,
 const glm::vec2& textureCoordinates) :
   position(position), normal(normal),
    tangent(tangent), bitangent(bitangent),
    textureCoordinates(textureCoordinates)
{}

Mesh::Component::Vertex::Vertex(const aiVector3D& position,
 const aiVector3D& normal, const aiVector3D& tangent,
 const aiVector3D& bitangent, const aiVector2D& textureCoordinates) :
   Vertex(glm::vec3(position.x, position.y, position.z),
    glm::vec3(normal.x, normal.y, normal.z),
    glm::vec3(tangent.x, tangent.y, tangent.z),
    glm::vec3(bitangent.x, bitangent.y, bitangent.z),
    glm::vec2(textureCoordinates.x, textureCoordinates.y))
{}

Mesh::Component::Vertex::Attribute::Attribute(
 unsigned int index, const std::string& name, size_t offset, size_t width) :
   index(index), name(name), offset(offset), width(width)
{}

/* static */ std::vector< Mesh::Component::Vertex::Attribute >
 Mesh::Component::Vertex::attributes = {{
   Attribute(0, "aPosition", offsetof(Vertex, position), 3),
   Attribute(1, "aNormal", offsetof(Vertex, normal), 3),
   Attribute(2, "aTangent", offsetof(Vertex, tangent), 3),
   Attribute(3, "aBitangent", offsetof(Vertex, bitangent), 3),
   Attribute(4, "aTexCoords", offsetof(Vertex, textureCoordinates), 2),
}};
