#pragma once

#include <string>
#include <assimp/scene.h>

namespace crash {
namespace render {

void printScene(const aiScene* scene, const std::string& prefix);

void printAnimation(const aiAnimation* animation, const std::string& prefix);
void printCamera(const aiCamera* camera, const std::string& prefix);
void printLight(const aiLight* light, const std::string& prefix);
void printMaterial(const aiMaterial* material, const std::string& prefix);
void printMesh(const aiMesh* mesh, const std::string& prefix);
void printTexture(const aiTexture* texture, const std::string& prefix);
void printNode(const aiNode* node, const std::string& prefix);

void printBoneChannel(const aiNodeAnim* node, const std::string& prefix);
void printMeshChannel(const aiMeshAnim* mesh, const std::string& prefix);

void printQuaternionKey(const aiQuatKey& key, const std::string& prefix);
void printVectorKey(const aiVectorKey& key, const std::string& prefix);
void printMeshKey(const aiMeshKey& key, const std::string& prefix);

void printFace(const aiFace& face, const std::string& prefix);
void printBone(const aiBone* bone, const std::string& prefix);

void printMatrix(const aiMatrix4x4& m, const std::string& prefix);
void printQuaternion(const aiQuaternion& q, const std::string& prefix);
void printVector(const aiVector3D& v, const std::string& prefix);
void printColor(const aiColor4D& c, const std::string& prefix);
void printColor(const aiColor3D& c, const std::string& prefix);

} // namespace render
} // namespace crash
