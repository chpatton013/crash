#include <iostream>
#include <vector>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <crash/common/arithmetic.hpp>
#include <crash/common/symbols.hpp>
#include <crash/common/util.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>
#include <crash/render/vertex.hpp>

using namespace crash::common;
using namespace crash::render;

Mesh::SceneImportFailure::SceneImportFailure(const std::string& error) :
   error(error)
{}

/* static */ const unsigned int Mesh::NUM_TEXTURE_TYPES = 6;

Mesh::Mesh(const Mesh& mesh) :
   _path(mesh._path), _scene(mesh._scene), _transformer(mesh._transformer),
    _boneNodes(), _animations(), _textureGroups(mesh._textureGroups),
    _vaos(mesh._vaos), _vbos(mesh._vbos), _ibos(mesh._ibos), _tbos(mesh._tbos),
    _components(mesh._components)
{}

Mesh::Mesh(const boost::filesystem::path& path) :
   _path(path), _scene(nullptr),
    _transformer(ORIGIN, NO_ROTATION, UNIT_SIZE,
    glm::vec3(), NO_ROTATION, glm::vec3()),
    _boneNodes(), _animations(), _textureGroups(),
    _vaos(), _vbos(), _ibos(), _tbos(), _components()
{
   const aiScene* scene = this->importScene();
   if (scene == nullptr) {
      throw SceneImportFailure(aiGetErrorString());
   }
   this->_scene = scene;

   this->normalizeScene();
   this->buildBoneNodeMap();
   this->buildAnimations();
   this->importTextures();
}

/* virtual */ Mesh::~Mesh() {
   this->releaseScene();
}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 Mesh::getPosition() const {
   return this->_transformer.getPosition();
}

glm::quat Mesh::getOrientation() const {
   return this->_transformer.getOrientation();
}

glm::vec3 Mesh::getSize() const {
   return this->_transformer.getSize();
}

glm::vec3 Mesh::getTranslationalVelocity() const {
   return this->_transformer.getTranslationalVelocity();
}

glm::quat Mesh::getRotationalVelocity() const {
   return this->_transformer.getRotationalVelocity();
}

glm::vec3 Mesh::getScaleVelocity() const {
   return this->_transformer.getScaleVelocity();
}

void Mesh::setPosition(const glm::vec3& position) {
   this->_transformer.setPosition(position);
}

void Mesh::setOrientation(const glm::quat& orientation) {
   this->_transformer.setOrientation(orientation);
}

void Mesh::setSize(const glm::vec3& size) {
   this->_transformer.setSize(size);
}

void Mesh::setTranslationalVelocity(const glm::vec3& translationalVelocity) {
   this->setTranslationalVelocity(translationalVelocity);
}

void Mesh::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->setRotationalVelocity(rotationalVelocity);
}

void Mesh::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->setScaleVelocity(scaleVelocity);
}

////////////////////////////////////////////////////////////////////////////////
// Rendering.
////////////////////////////////////////////////////////////////////////////////

const aiScene* Mesh::getScene() const {
   return this->_scene;
}

const std::vector< Animation >& Mesh::getAnimations() const {
   return this->_animations;
}

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

void Mesh::render(const ShaderProgram& program, const ColorUnit& color,
 const glm::mat4& parentTransform,
 const NodeTransformMap& nodeTransforms) const {
   glm::mat4 globalTransform = parentTransform * this->getTransform();
   this->renderNode(program, color, globalTransform, nodeTransforms,
    this->_scene->mRootNode);
}

/**
 * Post-processing flags:
 *    aiProcess_CalcTangentSpace
 *    aiProcess_Debone
 *    aiProcess_FixInfacingNormals
 *    aiProcess_FindDegenerates
 *    aiProcess_FindInstances
 *    aiProcess_FindInvalidData
 *    aiProcess_FlipUVs
 *    aiProcess_FlipWindingOrder
 *    aiProcess_GenSmoothNormals
 *    aiProcess_GenUVCoords
 *    aiProcess_ImproveCacheLocality
 *    aiProcess_JoinIdenticalVertices
 *    aiProcess_LimitBoneWeights
 *    aiProcess_MakeLeftHanded
 *    aiProcess_OptimizeMeshes
 *    aiProcess_OptimizeGraph
 *    aiProcess_RemoveRedundantMaterials
 *    aiProcess_SortByPType
 *    aiProcess_SplitLargeMeshes
 *    aiProcess_TransformUVCoords
 *    aiProcess_Triangulate
 *    aiProcess_ValidateDataStructure
 */
const aiScene* Mesh::importScene() {
   static const unsigned int POST_PROCESSING_FLAGS =
      aiProcessPreset_TargetRealtime_MaxQuality
    | aiProcess_ConvertToLeftHanded // Macro for FlipUVs, FlipWindingOrder, and MakeLeftHanded.
    | aiProcess_FixInfacingNormals
    | aiProcess_OptimizeGraph
    | aiProcess_TransformUVCoords;

   this->releaseScene();

   return aiImportFile(this->_path.string().data(), POST_PROCESSING_FLAGS);
}

void Mesh::normalizeScene() {
   glm::vec3 min(std::numeric_limits< float >::max());
   glm::vec3 max(std::numeric_limits< float >::min());
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      aiMesh* mesh = this->_scene->mMeshes[i];
      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
         glm::vec3 vertex = vec3AiToGlm(mesh->mVertices[j]);
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

void Mesh::buildAnimations() {
   this->_animations.clear();
   this->_animations.reserve(this->_scene->mNumAnimations);

   for (unsigned int i = 0; i < this->_scene->mNumAnimations; ++i) {
      this->_animations.push_back(Animation(this->_scene->mAnimations[i]));
   }
}

void Mesh::buildBoneNodeMap() {
   NodeNameMap nodeNames = this->getNodeNameMap();

   this->_boneNodes.clear();
   for (unsigned int i = 0; i < this->_scene->mNumMeshes; ++i) {
      const aiMesh* mesh = this->_scene->mMeshes[i];

      for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
         const aiBone* bone = mesh->mBones[j];
         std::string name = stringAiToStd(bone->mName);
         const aiNode* node = nodeNames.find(name)->second;
         this->_boneNodes.insert(std::make_pair(bone, node));
      }
   }
}

void Mesh::importTextures() {
   this->_textureGroups.clear();
   this->_textureGroups.reserve(this->_scene->mNumTextures);

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

void Mesh::buildComponents() {
   this->buildComponentsHelper(this->_scene->mRootNode);
}

void Mesh::releaseScene() {
   if (this->_scene == nullptr) {
      return;
   }

   aiReleaseImport(this->_scene);
   this->_scene = nullptr;
}

void Mesh::releaseBuffers() {
   glDeleteVertexArrays(this->_vaos.size(), this->_vaos.data());
   glDeleteBuffers(this->_vbos.size(), this->_vbos.data());
   glDeleteBuffers(this->_ibos.size(), this->_ibos.data());
   glDeleteTextures(this->_tbos.size(), this->_tbos.data());
}

void Mesh::destroyComponents() {
   this->_components.clear();
}

TexturePtr Mesh::importTexture(const aiMaterial* material,
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
    boost::filesystem::path(stringAiToStd(textureFileName));

   return std::make_shared< Texture >(Texture(texturePath));
}

void Mesh::buildComponentsHelper(const aiNode* node) {
   this->buildComponentHelper(node);

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->buildComponentsHelper(node->mChildren[i]);
   }
}

void Mesh::buildComponentHelper(const aiNode* node) {
   for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
      unsigned int meshIndex = node->mMeshes[i];
      aiMesh* mesh = this->_scene->mMeshes[meshIndex];

      unsigned int materialIndex = mesh->mMaterialIndex;
      aiMaterial* material = this->_scene->mMaterials[materialIndex];

      GeometryUnit geo(this->_vaos[meshIndex], this->_vbos[meshIndex],
       this->_ibos[meshIndex]);

      unsigned int tboOffset = materialIndex * Mesh::NUM_TEXTURE_TYPES;
      GLuint* tbos = this->_tbos.data() + tboOffset;

      TextureGroup texGroup = this->_textureGroups[materialIndex];
      TextureGroupUnit tex(
         TextureUnit(texGroup.displacement,  tbos[0], 0),
         TextureUnit(texGroup.normal,        tbos[1], 1),
         TextureUnit(texGroup.ambient,       tbos[2], 2),
         TextureUnit(texGroup.diffuse,       tbos[3], 3),
         TextureUnit(texGroup.specular,      tbos[4], 4),
         TextureUnit(texGroup.shininess,     tbos[5], 5)
      );

      this->_components.insert(std::make_pair(mesh,
       MeshComponent(node, mesh, material, geo, tex)));
   }
}

NodeNameMap Mesh::getNodeNameMap() const {
   NodeNameMap nodeNames;
   this->getNodeNameMapHelper(this->_scene->mRootNode, nodeNames);
   return nodeNames;
}

void Mesh::getNodeNameMapHelper(
 const aiNode* node, NodeNameMap& nodeNames) const {
   nodeNames.insert(std::make_pair(stringAiToStd(node->mName), node));

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->getNodeNameMapHelper(node->mChildren[i], nodeNames);
   }
}

void Mesh::renderNode(const ShaderProgram& program, const ColorUnit& color,
 const glm::mat4& globalTransform, const NodeTransformMap& nodeTransforms,
 const aiNode* node) const {
   glm::mat4 localTransform = nodeTransforms.find(node)->second;
   glm::mat4 modelTransform = globalTransform * localTransform;

   for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
      const aiMesh* mesh = this->_scene->mMeshes[node->mMeshes[i]];
      auto itr = this->_components.find(mesh);
      if (itr != this->_components.end()) {
         const MeshComponent& component = itr->second;
         component.render(program, color, modelTransform, this->_boneNodes,
          nodeTransforms);
      }
   }

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->renderNode(program, color, globalTransform, nodeTransforms,
       node->mChildren[i]);
   }
}
