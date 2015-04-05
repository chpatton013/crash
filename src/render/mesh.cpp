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

Mesh::SceneImportFailure::SceneImportFailure(const std::string& error) :
   error(error)
{}

////////////////////////////////////////////////////////////////////////////////
// Mesh
////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const Mesh& mesh) :
   _path(mesh._path), _scene(mesh._scene), _transformer(mesh._transformer),
    _vaos(mesh._vaos), _vbos(mesh._vbos), _ibos(mesh._ibos),
    _components(mesh._components)
{}

Mesh::Mesh(const boost::filesystem::path& path) :
   _path(path), _scene(nullptr),
    _transformer(glm::vec3(), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)),
    _vaos(), _vbos(), _ibos(), _components()
{
   this->importScene();
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

void Mesh::render(const ShaderProgram& program, MatrixStack& matrixStack) {
   matrixStack.push(this->getTransform());
   this->renderNode(program, matrixStack, this->_scene->mRootNode);
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

void Mesh::normalizeScene() {
   glm::vec3 min(std::numeric_limits< float >::max());
   glm::vec3 max(std::numeric_limits< float >::min());
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      aiMesh* mesh = this->_scene->mMeshes[i];
      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
         aiVector3D vertex = mesh->mVertices[j];
         min.x = std::min< float >(min.x, vertex.x);
         min.y = std::min< float >(min.y, vertex.y);
         min.z = std::min< float >(min.z, vertex.z);
         max.x = std::max< float >(max.x, vertex.x);
         max.y = std::max< float >(max.y, vertex.y);
         max.z = std::max< float >(max.z, vertex.z);
      }
   }

   glm::vec3 dimensions = glm::abs(max - min);
   float maxDimension = std::max< float >(
    std::max< float >(dimensions.x, dimensions.y), dimensions.z);
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
      auto mesh = this->_scene->mMeshes[i];
      auto vao = this->_vaos[i];
      auto vbo = this->_vbos[i];
      auto ibo = this->_ibos[i];
      this->_components.insert(std::make_pair(mesh,
       Component(mesh, vao, vbo, ibo)));
   }
}

void Mesh::allocateBuffers() {
   unsigned int numBuffers = this->_scene->mNumMeshes;

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
}

void Mesh::renderNode(const ShaderProgram& program, MatrixStack& matrixStack,
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
         component.render(program, matrixStack.top());
      }
   }

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->renderNode(program, matrixStack, node->mChildren[i]);
   }

   matrixStack.pop();
}

////////////////////////////////////////////////////////////////////////////////
// Mesh::Component
////////////////////////////////////////////////////////////////////////////////

Mesh::Component::Component(const Component& component) :
   mesh(component.mesh), vao(component.vao), vbo(component.vbo), ibo(component.ibo)
{}

Mesh::Component::Component(const aiMesh* mesh, const GLuint& vao,
 const GLuint& vbo, const GLuint& ibo) :
   mesh(mesh), vao(vao), vbo(vbo), ibo(ibo)
{
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
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

      auto textureCoordinates = zero;
      if (this->mesh->HasTextureCoords(0)) {
         textureCoordinates = this->mesh->mTextureCoords[0][i];
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

void Mesh::Component::bindAttributes(const ShaderProgram& program) const {
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBindVertexArray(this->vao);
   for (auto& attribute : Mesh::Component::Vertex::attributes) {
      glBindAttribLocation(program.getHandle(), attribute.index,
       attribute.name.data());
   }
}

void Mesh::Component::render(const ShaderProgram& program,
 const glm::mat4& transform) const {
   program.setUniformVariableMatrix4("uMvpMatrix", glm::value_ptr(transform), 1);

   glBindVertexArray(this->vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

   glDrawElements(GL_TRIANGLES, this->mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
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
 const glm::vec3& textureCoordinates) :
   position(position), normal(normal),
    tangent(tangent), bitangent(bitangent),
    textureCoordinates(textureCoordinates)
{}

Mesh::Component::Vertex::Vertex(const aiVector3D& position,
 const aiVector3D& normal, const aiVector3D& tangent,
 const aiVector3D& bitangent, const aiVector3D& textureCoordinates) :
   Vertex(glm::vec3(position.x, position.y, position.z),
    glm::vec3(normal.x, normal.y, normal.z),
    glm::vec3(tangent.x, tangent.y, tangent.z),
    glm::vec3(bitangent.x, bitangent.y, bitangent.z),
    glm::vec3(textureCoordinates.x, textureCoordinates.y, textureCoordinates.z))
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
   Attribute(4, "aTexCoords", offsetof(Vertex, textureCoordinates), 3),
}};
