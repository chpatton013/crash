#include <vector>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <crash/math/arithmetic.hpp>
#include <crash/math/symbols.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/vertex.hpp>

using namespace crash::math;
using namespace crash::render;

Mesh::SceneImportFailure::SceneImportFailure(const std::string& error) :
   error(error)
{}

/* static */ const unsigned int Mesh::NUM_TEXTURE_TYPES = 6;

Mesh::Mesh(const Mesh& mesh) :
   _path(mesh._path), _scene(mesh._scene), _transformer(mesh._transformer),
    _vaos(mesh._vaos), _vbos(mesh._vbos), _ibos(mesh._ibos), _tbos(mesh._tbos),
    _components(mesh._components), _textureGroups(mesh._textureGroups)
{}

Mesh::Mesh(const boost::filesystem::path& path) :
   _path(path), _scene(nullptr),
    _transformer(glm::vec3(), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)),
    _vaos(), _vbos(), _ibos(), _tbos(), _components(), _textureGroups()
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

void Mesh::bindAttributes(const ShaderProgram& program,
 const AttributeVariable& vars) const {
   for (auto itr : this->_components) {
      itr.second.bindAttributes(program, vars);
   }
}

void Mesh::render(const ShaderProgram& program, const UniformVariable& vars,
 MatrixStack& matrixStack) {
   matrixStack.push(this->getTransform());
   this->renderNode(program, vars, matrixStack, this->_scene->mRootNode);
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

void Mesh::releaseScene() {
   if (this->_scene == nullptr) {
      return;
   }

   aiReleaseImport(this->_scene);
   this->_scene = nullptr;
}

void Mesh::importTextures() {
   for (unsigned int i = 0; i < this->_scene->mNumMaterials; ++i) {
      auto displacement = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_DISPLACEMENT, /* index */ 0);
      auto normal = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_NORMALS, /* index */ 0);
      auto ambient = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_AMBIENT, /* index */ 0);
      auto diffuse = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_DIFFUSE, /* index */ 0);
      auto specular = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_SPECULAR, /* index */ 0);
      auto shininess = this->importTexture(this->_scene->mMaterials[i],
       aiTextureType_SHININESS, /* index */ 0);
      this->_textureGroups.push_back(TextureGroup(displacement, normal,
       ambient, diffuse, specular, shininess));
   }
}

std::shared_ptr< Texture > Mesh::importTexture(const aiMaterial* material,
 const aiTextureType& type, unsigned int index) {
   aiString textureFileName;
   aiReturn ret = material->GetTexture(type, index, &textureFileName,
    /* mapping */ nullptr, /* uv index */ nullptr, /* blend */ nullptr,
    /* operator */ nullptr, /* map mode */ nullptr);
   if (ret != aiReturn_SUCCESS) {
      return nullptr;
   }

   boost::filesystem::path texturePath =
    boost::filesystem::path(this->_path).parent_path() /
    boost::filesystem::path(textureFileName.C_Str());

   return std::make_shared< Texture >(Texture(texturePath));
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

void Mesh::buildComponents() {
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      aiMesh* mesh = this->_scene->mMeshes[i];
      aiMaterial* material = this->_scene->mMaterials[mesh->mMaterialIndex];

      GeometryUnit geo(this->_vaos[i], this->_vbos[i], this->_ibos[i]);

      unsigned int tboOffset = mesh->mMaterialIndex *
       Mesh::NUM_TEXTURE_TYPES;
      GLuint* tbos = this->_tbos.data() + tboOffset;

      TextureGroup texGroup = this->_textureGroups[mesh->mMaterialIndex];
      TextureGroupUnit tex(
         TextureUnit(texGroup.displacement,  tbos[0], 0),
         TextureUnit(texGroup.normal,        tbos[1], 1),
         TextureUnit(texGroup.ambient,       tbos[2], 2),
         TextureUnit(texGroup.diffuse,       tbos[3], 3),
         TextureUnit(texGroup.specular,      tbos[4], 4),
         TextureUnit(texGroup.shininess,     tbos[5], 5)
      );

      this->_components.insert(std::make_pair(mesh,
       MeshComponent(mesh, material, geo, tex)));
   }
}

void Mesh::allocateBuffers() {
   unsigned int numBuffers = this->_scene->mNumMeshes;
   unsigned int numTextures = this->_scene->mNumMaterials *
    Mesh::NUM_TEXTURE_TYPES;

   this->_vaos.clear();
   this->_vaos.resize(numBuffers, 0);
   glGenVertexArrays(numBuffers, this->_vaos.data());

   this->_vbos.clear();
   this->_vbos.resize(numBuffers, 0);
   glGenBuffers(numBuffers, this->_vbos.data());

   this->_ibos.clear();
   this->_ibos.resize(numBuffers, 0);
   glGenBuffers(numBuffers, this->_ibos.data());

   this->_tbos.clear();
   this->_tbos.resize(numTextures, 0);
   glGenTextures(numTextures, this->_tbos.data());
}

void Mesh::destroyComponents() {
   this->_components.clear();
}

void Mesh::releaseBuffers() {
   glDeleteVertexArrays(this->_vaos.size(), this->_vaos.data());
   glDeleteBuffers(this->_vbos.size(), this->_vbos.data());
   glDeleteBuffers(this->_ibos.size(), this->_ibos.data());
   glDeleteTextures(this->_tbos.size(), this->_tbos.data());
}

void Mesh::renderNode(const ShaderProgram& program,
 const UniformVariable& vars, MatrixStack& matrixStack,
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
         component.render(program, vars, matrixStack.top());
      }
   }

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->renderNode(program, vars, matrixStack, node->mChildren[i]);
   }

   matrixStack.pop();
}
