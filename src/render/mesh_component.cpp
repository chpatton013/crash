#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <crash/math/util.hpp>
#include <crash/render/matrix_stack.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/mesh_component.hpp>
#include <crash/render/shader_program.hpp>
#include <crash/render/util.hpp>

using namespace crash::math;
using namespace crash::render;

ColorUnit::ColorUnit(const glm::vec4& ambient, const glm::vec4& diffuse,
 const glm::vec4& specular, float shininess) :
   ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{}

MaterialUnit::MaterialUnit(const ColorUnit& color, bool twoSided) :
   color(color), twoSided(twoSided)
{}

GeometryUnit::GeometryUnit(const GLuint& vao, const GLuint& vbo,
 const GLuint& ibo) :
   vao(vao), vbo(vbo), ibo(ibo)
{}

TextureUnit::TextureUnit(std::shared_ptr< Texture > texture, const GLuint& tbo,
 const GLint& index) :
   texture(texture), tbo(tbo), index(index)
{}

TextureGroupUnit::TextureGroupUnit(
 const TextureUnit& displacement, const TextureUnit& normal,
 const TextureUnit& ambient, const TextureUnit& diffuse,
 const TextureUnit& specular, const TextureUnit& shininess) :
   displacement(displacement), normal(normal),
    ambient(ambient), diffuse(diffuse),
    specular(specular), shininess(shininess)
{}

BoneWeightUnit::BoneWeightUnit(unsigned int index, float weight) :
   index(index), weight(weight)
{}

BoneTransformUnit::BoneTransformUnit(
 const glm::mat4& offset, const glm::mat4& transform) :
   offset(offset), transform(transform)
{}

/* static */ const glm::vec4 MeshComponent::DEFAULT_AMBIENT_COLOR =
 glm::vec4(glm::vec3(0.1f), 1.0f);
/* static */ const glm::vec4 MeshComponent::DEFAULT_DIFFUSE_COLOR =
 glm::vec4(glm::vec3(0.7f), 1.0f);
/* static */ const glm::vec4 MeshComponent::DEFAULT_SPECULAR_COLOR =
 glm::vec4(glm::vec3(0.9f), 1.0f);
/* static */ const float MeshComponent::DEFAULT_SHININESS_VALUE = 250.0f;

/* static */ const glm::vec4 MeshComponent::DEFAULT_AMBIENT_BASE_COLOR =
 glm::vec4(1.0f);
/* static */ const glm::vec4 MeshComponent::DEFAULT_DIFFUSE_BASE_COLOR =
 glm::vec4(1.0f);
/* static */ const glm::vec4 MeshComponent::DEFAULT_SPECULAR_BASE_COLOR =
 glm::vec4(1.0f);
/* static */ const float MeshComponent::DEFAULT_SHININESS_BASE_VALUE = 1.0f;

MeshComponent::MeshComponent(const MeshComponent& component) :
   _node(component._node), _mesh(component._mesh),
    _material(component._material), _materialUnit(component._materialUnit),
    _geometryUnit(component._geometryUnit),
    _textureGroupUnit(component._textureGroupUnit)
{}

MeshComponent::MeshComponent(const aiNode* node, const aiMesh* mesh,
 const aiMaterial* material, const GeometryUnit& geometryUnit,
 const TextureGroupUnit& textureGroupUnit) :
   _node(node), _mesh(mesh), _material(material),
    _materialUnit(MeshComponent::extractMaterialUnit(material)),
    _geometryUnit(geometryUnit), _textureGroupUnit(textureGroupUnit)
{
   this->fillVertexBones();
   this->generateVertexBuffer();
   this->generateIndexBuffer();
   this->generateVertexArray();
   this->generateTextureBuffers();
}

void MeshComponent::bindAttributes(const ShaderProgram& program,
 const AttributeVariable& vars) const {
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindVertexArray(this->_geometryUnit.vao);

   Vertex::bindAttributes(program, vars);
}

void MeshComponent::render(const ShaderProgram& program,
 const UniformVariable& vars, const ColorUnit& color,
 const glm::mat4& modelTransform,
 const BoneNodeMap& boneNodes, const NodeTransformMap& nodeTransforms) const {
   program.setUniformVariableMatrix4(vars.model_transform,
    glm::value_ptr(modelTransform), 1);

   this->activateBones(program, vars, boneNodes, nodeTransforms);
   this->activateMaterial(program, vars, color);
   this->activateTextures(program, vars);
   this->activateGeometry();

   glDrawElements(GL_TRIANGLES, this->_mesh->mNumFaces * 3, GL_UNSIGNED_INT,
    reinterpret_cast< const GLvoid* >(0));
}

void MeshComponent::fillVertexBones() {
   this->_vertexBones.clear();
   this->_vertexBones.resize(this->_mesh->mNumVertices);

   for (unsigned int i = 0; i < this->_mesh->mNumBones; i++) {
      const aiBone* bone = this->_mesh->mBones[i];

      for (unsigned int j = 0; j < bone->mNumWeights; ++j) {
         const aiVertexWeight* vertexWeight = bone->mWeights + j;
         float weight = vertexWeight->mWeight;

         if (weight > 0.0f) {
            unsigned int vertex = vertexWeight->mVertexId;
            this->_vertexBones[vertex].push_back(BoneWeightUnit(i, weight));
         }
      }
   }
}

void MeshComponent::generateVertexArray() const {
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindVertexArray(this->_geometryUnit.vao);

   Vertex::defineAttributes();
}

void MeshComponent::generateVertexBuffer() const {
   std::vector< Vertex > vertices;
   vertices.reserve(this->_mesh->mNumVertices);

   for (unsigned int i = 0; i < this->_mesh->mNumVertices; ++i) {
      glm::vec3 position = this->getVertexPosition(i);
      glm::vec3 normal = this->getVertexNormal(i);

      glm::vec3 tangent;
      glm::vec3 bitangent;
      std::tie(tangent, bitangent) = this->getVertexTangentAndBitangent(i);

      glm::vec2 textureCoordinates = this->getVertexTextureCoordinates(i);

      glm::ivec4 boneIds;
      glm::vec4 boneWeights;
      std::tie(boneIds, boneWeights) = this->getVertexBones(i);

      vertices.push_back(Vertex(position, normal, tangent, bitangent,
       textureCoordinates, boneIds, boneWeights));
   }

   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
    vertices.data(), GL_STATIC_DRAW);
}

void MeshComponent::generateIndexBuffer() const {
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

void MeshComponent::generateTextureBuffers() const {
   this->generateTextureBuffer(this->_textureGroupUnit.displacement);
   this->generateTextureBuffer(this->_textureGroupUnit.normal);
   this->generateTextureBuffer(this->_textureGroupUnit.ambient);
   this->generateTextureBuffer(this->_textureGroupUnit.diffuse);
   this->generateTextureBuffer(this->_textureGroupUnit.specular);
   this->generateTextureBuffer(this->_textureGroupUnit.ambient);
}

void MeshComponent::activateBones(const ShaderProgram& program,
 const UniformVariable& vars, const BoneNodeMap& boneNodes,
 const NodeTransformMap& nodeTransforms) const {
   glm::mat4 rootToMeshTransform = nodeTransforms.find(this->_node)->second;
   glm::mat4 meshToRootTransform = glm::inverse(rootToMeshTransform);

   std::vector< glm::mat4 > transforms;
   transforms.reserve(this->_mesh->mNumBones);

   for (unsigned int i = 0; i < this->_mesh->mNumBones; ++i) {
      const aiBone* bone = this->_mesh->mBones[i];
      const aiNode* node = boneNodes.find(bone)->second;
      glm::mat4 rootToBoneTransform = nodeTransforms.find(node)->second;
      glm::mat4 meshToBoneTransform = meshToRootTransform * rootToBoneTransform;
      glm::mat4 meshOffset = mat4AiToGlm(bone->mOffsetMatrix);
      transforms.push_back(meshToBoneTransform * meshOffset);
   }

   program.setUniformVariableMatrix4(vars.bones,
    glm::value_ptr(*transforms.data()), transforms.size());
}

void MeshComponent::activateMaterial(const ShaderProgram& program,
 const UniformVariable& vars, const ColorUnit& color) const {
   program.setUniformVariable4f(vars.ambient_color,
    glm::value_ptr(this->_materialUnit.color.ambient), 1);
   program.setUniformVariable4f(vars.diffuse_color,
    glm::value_ptr(this->_materialUnit.color.diffuse), 1);
   program.setUniformVariable4f(vars.specular_color,
    glm::value_ptr(this->_materialUnit.color.specular), 1);
   program.setUniformVariable1f(vars.shininess_value,
    &this->_materialUnit.color.shininess, 1);

   program.setUniformVariable4f(vars.ambient_base_color,
    glm::value_ptr(color.ambient), 1);
   program.setUniformVariable4f(vars.diffuse_base_color,
    glm::value_ptr(color.diffuse), 1);
   program.setUniformVariable4f(vars.specular_base_color,
    glm::value_ptr(color.specular), 1);
   program.setUniformVariable1f(vars.shininess_base_value,
    &color.shininess, 1);

   if (this->_materialUnit.twoSided) {
      glDisable(GL_CULL_FACE);
   } else {
      glEnable(GL_CULL_FACE);
   }
}

void MeshComponent::activateTextures(const ShaderProgram& program,
 const UniformVariable& vars) const {
   this->activateTexture(program, vars.has_displacement_texture,
    vars.displacement_texture, this->_textureGroupUnit.displacement);
   this->activateTexture(program, vars.has_normal_texture,
    vars.normal_texture, this->_textureGroupUnit.normal);
   this->activateTexture(program, vars.has_ambient_texture,
    vars.ambient_texture, this->_textureGroupUnit.ambient);
   this->activateTexture(program, vars.has_diffuse_texture,
    vars.diffuse_texture, this->_textureGroupUnit.diffuse);
   this->activateTexture(program, vars.has_specular_texture,
    vars.specular_texture, this->_textureGroupUnit.specular);
   this->activateTexture(program, vars.has_shininess_texture,
    vars.shininess_texture, this->_textureGroupUnit.shininess);
}

void MeshComponent::activateGeometry() const {
   glBindVertexArray(this->_geometryUnit.vao);
   glBindBuffer(GL_ARRAY_BUFFER, this->_geometryUnit.vbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_geometryUnit.ibo);
}

glm::vec3 MeshComponent::getVertexPosition(unsigned int index) const {
   return vec3AiToGlm(this->_mesh->mVertices[index]);
}

glm::vec3 MeshComponent::getVertexNormal(unsigned int index) const {
   if (!this->_mesh->HasNormals()) {
      return glm::vec3();
   }

   return vec3AiToGlm(this->_mesh->mNormals[index]);
}

std::tuple< glm::vec3, glm::vec3 >
 MeshComponent::getVertexTangentAndBitangent(unsigned int index) const {
   if (!this->_mesh->HasTangentsAndBitangents()) {
      return std::make_tuple(glm::vec3(), glm::vec3());
   }

   glm::vec3 tangent = vec3AiToGlm(this->_mesh->mTangents[index]);
   glm::vec3 bitangent = vec3AiToGlm(this->_mesh->mBitangents[index]);
   return std::make_tuple(tangent, bitangent);
}

glm::vec2 MeshComponent::getVertexTextureCoordinates(unsigned int index) const {
   if (!this->_mesh->HasTextureCoords(0)) {
      return glm::vec2();
   }

   return glm::vec2(vec3AiToGlm(this->_mesh->mTextureCoords[0][index]));
}

std::tuple< glm::ivec4, glm::vec4 >
 MeshComponent::getVertexBones(unsigned int index) const {
   const BoneWeightGroupUnit& bone = this->_vertexBones[index];

   glm::ivec4 boneIds(0);
   glm::vec4 boneWeights(0.0f);

   for (unsigned int i = 0; i < bone.size(); ++i) {
      boneIds[i] = bone[i].index;
      boneWeights[i] = bone[i].weight;
   }

   return std::make_tuple(boneIds, boneWeights);
}

void MeshComponent::generateTextureBuffer(
 const TextureUnit& textureUnit) const {
   if (textureUnit.texture == nullptr) {
      return;
   }

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

void MeshComponent::activateTexture(const ShaderProgram& program,
 const std::string& hasTextureVar, const std::string& textureVar,
 const TextureUnit& textureUnit) const {
   if (textureUnit.texture == nullptr) {
      GLuint hasTexture = 0;
      program.setUniformVariable1ui(hasTextureVar, &hasTexture, 1);
   } else {
      GLuint hasTexture = 1;
      program.setUniformVariable1ui(hasTextureVar, &hasTexture, 1);

      glActiveTexture(GL_TEXTURE0 + textureUnit.index);
      glBindTexture(GL_TEXTURE_2D, textureUnit.tbo);
      program.setUniformVariable1i(textureVar, &textureUnit.index, 1);
   }
}

/* static */ MaterialUnit MeshComponent::extractMaterialUnit(
 const aiMaterial* material) {
   glm::vec4 ambient = MeshComponent::DEFAULT_AMBIENT_COLOR;
   glm::vec4 diffuse = MeshComponent::DEFAULT_DIFFUSE_COLOR;
   glm::vec4 specular = MeshComponent::DEFAULT_SPECULAR_COLOR;
   float shininess = MeshComponent::DEFAULT_SHININESS_VALUE;
   bool twoSided = false;

   if (material == nullptr) {
      return MaterialUnit(ColorUnit(ambient, diffuse, specular, shininess),
       twoSided);
   }

   aiColor4D aiAmbient(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);
   if (!aiAmbient.IsBlack()) {
      ambient = color4AiToGlm(aiAmbient);
   }

   aiColor4D aiDiffuse(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
   if (!aiDiffuse.IsBlack()) {
      diffuse = color4AiToGlm(aiDiffuse);
   }

   aiColor4D aiSpecular(0.0f, 0.0f, 0.0f, 1.0f);
   material->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);
   if (!aiSpecular.IsBlack()) {
      specular = color4AiToGlm(aiSpecular);
   }

   float aiShininess = 0.0f;
   material->Get(AI_MATKEY_SHININESS, aiShininess);
   if (aiShininess != 0.0f) {
      shininess = aiShininess;
   }

   int aiTwoSided;
   material->Get(AI_MATKEY_TWOSIDED, aiTwoSided);
   twoSided = (aiTwoSided != 0);

   return MaterialUnit(ColorUnit(ambient, diffuse, specular, shininess),
    twoSided);
}
