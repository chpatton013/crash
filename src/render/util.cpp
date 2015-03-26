#include <iostream>
#include <string>
#include <assimp/scene.h>
#include <crash/render/util.hpp>

void crash::render::printScene(const aiScene* scene, const std::string& prefix) {
   std::cout << prefix << "animations: " << scene->mNumAnimations << std::endl;
   for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
      printAnimation(scene->mAnimations[i], prefix + "   ");
   }

   std::cout << prefix << "cameras: " << scene->mNumCameras << std::endl;
   for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
      printCamera(scene->mCameras[i], prefix + "   ");
   }

   std::cout << prefix << "flags: " <<
    std::hex << std::showbase << scene->mFlags << std::noshowbase << std::dec <<
    std::endl;

   std::cout << prefix << "lights: " << scene->mNumLights << std::endl;
   for (unsigned int i = 0; i < scene->mNumLights; ++i) {
      printLight(scene->mLights[i], prefix + "   ");
   }

   std::cout << prefix << "materials: " << scene->mNumMaterials << std::endl;
   for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
      printMaterial(scene->mMaterials[i], prefix + "   ");
   }

   std::cout << prefix << "meshes: " << scene->mNumMeshes << std::endl;
   for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
      std::cout << prefix << "   " << i << std::endl;
      printMesh(scene->mMeshes[i], prefix + "      ");
   }

   std::cout << prefix << "textures: " << scene->mNumTextures << std::endl;
   for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
      printTexture(scene->mTextures[i], prefix + "   ");
   }

   std::cout << prefix << "nodes:" << std::endl;
   printNode(scene->mRootNode, prefix + "   ");
}

void crash::render::printAnimation(const aiAnimation* animation, const std::string& prefix) {
   std::cout << prefix << "name: " << animation->mName.C_Str() << std::endl;

   std::cout << prefix << "duration: " << animation->mDuration << std::endl;
   std::cout << prefix << "frequency: " << animation->mTicksPerSecond << std::endl;

   std::cout << prefix << "bone channels: " << animation->mNumChannels << std::endl;
   for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
      printBoneChannel(animation->mChannels[i], prefix + "   ");
   }

   std::cout << prefix << "mesh channels: " << animation->mNumMeshChannels << std::endl;
   for (unsigned int i = 0; i < animation->mNumMeshChannels; ++i) {
      printMeshChannel(animation->mMeshChannels[i], prefix + "   ");
   }
}

void crash::render::printCamera(const aiCamera* camera, const std::string& prefix) {
   std::cout << prefix << "name:" << camera->mName.C_Str() << std::endl;

   std::cout << prefix << "position:" << std::endl;
   printVector(camera->mPosition, prefix + "   ");

   std::cout << prefix << "look at:" << std::endl;
   printVector(camera->mLookAt, prefix + "   ");

   std::cout << prefix << "up:" << std::endl;
   printVector(camera->mUp, prefix + "   ");

   std::cout << prefix << "horizontal FOV:" << camera->mHorizontalFOV << std::endl;
   std::cout << prefix << "aspect ratio:" << camera->mAspect << std::endl;
   std::cout << prefix << "near plane:" << camera->mClipPlaneNear << std::endl;
   std::cout << prefix << "far plane:" << camera->mClipPlaneFar << std::endl;
}

void crash::render::printLight(const aiLight* light, const std::string& prefix) {
   auto typeToString = [](const aiLightSourceType& type)->std::string {
      switch (type) {
         case aiLightSource_UNDEFINED: return "undefined";
         case aiLightSource_DIRECTIONAL: return "directional";
         case aiLightSource_POINT: return "point";
         case aiLightSource_SPOT: return "spot";
         default: return "unknown";
      }
   };

   std::cout << prefix << "name:" << light->mName.C_Str() << std::endl;
   std::cout << prefix << "type:" << typeToString(light->mType) << std::endl;

   std::cout << prefix << "position:" << std::endl;
   printVector(light->mPosition, prefix + "   ");
   std::cout << prefix << "direction:" << std::endl;
   printVector(light->mDirection, prefix + "   ");

   std::cout << prefix << "ambient:" << std::endl;
   printColor(light->mColorAmbient, prefix + "   ");
   std::cout << prefix << "diffuse:" << std::endl;
   printColor(light->mColorDiffuse, prefix + "   ");
   std::cout << prefix << "specular:" << std::endl;
   printColor(light->mColorSpecular, prefix + "   ");

   std::cout << prefix << "inner angle: " << light->mAngleInnerCone << std::endl;
   std::cout << prefix << "outer angle: " << light->mAngleOuterCone << std::endl;

   std::cout << prefix << "attenuation constant: " << light->mAttenuationConstant << std::endl;
   std::cout << prefix << "attenuation linear: " << light->mAttenuationLinear << std::endl;
   std::cout << prefix << "attenuation quadratic: " << light->mAttenuationQuadratic << std::endl;
}

void crash::render::printMaterial(const aiMaterial* material, const std::string& prefix) {
   std::cout << prefix << "properties: " << material->mNumProperties << std::endl;
   for (unsigned int i = 0; i < material->mNumProperties; ++i) {
      std::cout << prefix << "   " << material->mProperties[i]->mKey.C_Str() <<
       "(" << material->mProperties[i]->mDataLength << ")" << std::endl;
   }
}

void crash::render::printMesh(const aiMesh* mesh, const std::string& prefix) {
   std::cout << prefix << "name: " << mesh->mName.C_Str() << std::endl;

   std::cout << prefix << "vertices: " << mesh->mNumVertices << std::endl;
   for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      std::cout << prefix << "   " << i << std::endl;
      std::cout << prefix << "      point: ";
      printVector(mesh->mVertices[i], "");

      if (mesh->mNormals != NULL) {
         std::cout << prefix << "      normal: ";
         printVector(mesh->mNormals[i], "");
      }

      if (mesh->mTangents != NULL) {
         std::cout << prefix << "      tangent: ";
         printVector(mesh->mTangents[i], "");
      }

      if (mesh->mBitangents != NULL) {
         std::cout << prefix << "      bitangent: ";
         printVector(mesh->mBitangents[i], "");
      }
   }

   for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; ++i) {
      if (mesh->mColors[i] == NULL) {
         continue;
      }

      std::cout << prefix << "color: ";
      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
         printColor(mesh->mColors[i][j], prefix + "   ");
      }
   }

   for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i) {
      if (mesh->mTextureCoords[i] == NULL) {
         continue;
      }

      std::cout << prefix << "texture coordinates: (" << mesh->mNumUVComponents[i] << ")" << std::endl;
      for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
         printVector(mesh->mTextureCoords[i][j], prefix + "   ");
      }
   }

   std::cout << prefix << "faces: " << mesh->mNumFaces << std::endl;
   for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      printFace(mesh->mFaces[i], prefix + "   ");
   }

   std::cout << prefix << "bones: " << mesh->mNumBones << std::endl;
   for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
      printBone(mesh->mBones[i], prefix + "   ");
   }

   std::cout << prefix << "material index: " << mesh->mMaterialIndex << std::endl;

   std::cout << prefix << "primitive types:" << std::endl;
   if (mesh->mPrimitiveTypes | aiPrimitiveType_POINT) {
      std::cout << prefix << "   " << "point" << std::endl;
   }
   if (mesh->mPrimitiveTypes | aiPrimitiveType_LINE) {
      std::cout << prefix << "   " << "line" << std::endl;
   }
   if (mesh->mPrimitiveTypes | aiPrimitiveType_TRIANGLE) {
      std::cout << prefix << "   " << "triangle" << std::endl;
   }
   if (mesh->mPrimitiveTypes | aiPrimitiveType_POLYGON) {
      std::cout << prefix << "   " << "polygon" << std::endl;
   }
}

void crash::render::printTexture(const aiTexture* texture, const std::string& prefix) {
   std::cout << prefix << "height: " << texture->mHeight << std::endl;
   std::cout << prefix << "width: " << texture->mWidth << std::endl;
}

void crash::render::printNode(const aiNode* node, const std::string& prefix) {
   std::cout << prefix << "name: " << node->mName.C_Str() << std::endl;

   std::cout << prefix << "transformation:" << std::endl;
   printMatrix(node->mTransformation, prefix + "   ");

   std::cout << prefix << "mesh handles: " << node->mNumMeshes << std::endl;
   if (node->mNumMeshes > 0) {
      std::cout << prefix << "   ";
      for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
         std::cout << node->mMeshes[i] << ",";
      }
      std::cout << std::endl;
   }

   std::cout << prefix << "children: " << node->mNumChildren << std::endl;
   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      printNode(node->mChildren[i], prefix + "   ");
   }
}

void crash::render::printBoneChannel(const aiNodeAnim* node, const std::string& prefix) {
   auto behaviourToString = [](const aiAnimBehaviour& behaviour)->std::string {
      switch (behaviour) {
         case aiAnimBehaviour_DEFAULT: return "default";
         case aiAnimBehaviour_CONSTANT: return "constant";
         case aiAnimBehaviour_LINEAR: return "linear";
         case aiAnimBehaviour_REPEAT: return "repeat";
         default: return "unknown";
      }
   };

   std::cout << prefix << "name: " << node->mNodeName.C_Str() << std::endl;

   std::cout << prefix << "position key frames: " << node->mNumPositionKeys << std::endl;
   for (unsigned int i = 0; i < node->mNumPositionKeys; ++i) {
      printVectorKey(node->mPositionKeys[i], prefix + "   ");
   }
   std::cout << prefix << "rotation key frames: " << node->mNumRotationKeys << std::endl;
   for (unsigned int i = 0; i < node->mNumRotationKeys; ++i) {
      printQuaternionKey(node->mRotationKeys[i], prefix + "   ");
   }
   std::cout << prefix << "scaling key frames: " << node->mNumScalingKeys << std::endl;
   for (unsigned int i = 0; i < node->mNumScalingKeys; ++i) {
      printVectorKey(node->mScalingKeys[i], prefix + "   ");
   }

   std::cout << prefix << "pre state: " << behaviourToString(node->mPreState) << std::endl;
   std::cout << prefix << "post state: " << behaviourToString(node->mPostState) << std::endl;
}

void crash::render::printMeshChannel(const aiMeshAnim* mesh, const std::string& prefix) {
   std::cout << prefix << "name: " << mesh->mName.C_Str() << std::endl;

   std::cout << prefix << "key frames: " << mesh->mNumKeys << std::endl;
   for (unsigned int i = 0; i < mesh->mNumKeys; ++i) {
      printMeshKey(mesh->mKeys[i], prefix + "   ");
   }
}

void crash::render::printQuaternionKey(const aiQuatKey& key, const std::string& prefix) {
   std::cout << prefix << "time: " << key.mTime << std::endl;
   std::cout << prefix << "value:" << std::endl;
   printQuaternion(key.mValue, prefix + "   ");
}

void crash::render::printVectorKey(const aiVectorKey& key, const std::string& prefix) {
   std::cout << prefix << "time: " << key.mTime << std::endl;
   std::cout << prefix << "value:" << std::endl;
   printVector(key.mValue, prefix + "   ");
}

void crash::render::printMeshKey(const aiMeshKey& key, const std::string& prefix) {
   std::cout << prefix << "mesh handle: " << key.mValue << std::endl;
   std::cout << prefix << "time: " << key.mTime << std::endl;
}

void crash::render::printFace(const aiFace& face, const std::string& prefix) {
   std::cout << prefix;
   for (unsigned int i = 0; i < face.mNumIndices; ++i) {
      std::cout << face.mIndices[i] << ",";
   }
   std::cout << std::endl;
}

void crash::render::printBone(const aiBone* bone, const std::string& prefix) {
   std::cout << prefix << "name: " << bone->mName.C_Str() << std::endl;

   std::cout << prefix << "offset:" << std::endl;
   printMatrix(bone->mOffsetMatrix, prefix + "   ");

   std::cout << prefix << "weights:" << std::endl;
   for (unsigned int i = 0; i < bone->mNumWeights; ++i) {
      std::cout << prefix << "   vertex: " << bone->mWeights[i].mVertexId << std::endl;
      std::cout << prefix << "   weight: " << bone->mWeights[i].mWeight << std::endl;
   }
}

void crash::render::printMatrix(const aiMatrix4x4& m, const std::string& prefix) {
   std::cout << prefix << m.a1 << "," << m.a2 << "," << m.a3 << "," << m.a4 << std::endl;
   std::cout << prefix << m.b1 << "," << m.b2 << "," << m.b3 << "," << m.b4 << std::endl;
   std::cout << prefix << m.c1 << "," << m.c2 << "," << m.c3 << "," << m.c4 << std::endl;
   std::cout << prefix << m.d1 << "," << m.d2 << "," << m.d3 << "," << m.d4 << std::endl;
}

void crash::render::printQuaternion(const aiQuaternion& q, const std::string& prefix) {
   std::cout << prefix << q.w << "," << q.x << "," << q.y << "," << q.z << std::endl;
}

void crash::render::printVector(const aiVector3D& v, const std::string& prefix) {
   std::cout << prefix << v.x << "," << v.y << "," << v.z << std::endl;
};

void crash::render::printColor(const aiColor4D& c, const std::string& prefix) {
   std::cout << prefix << c.r << "," << c.g << "," << c.b << "," << c.a << std::endl;
}

void crash::render::printColor(const aiColor3D& c, const std::string& prefix) {
   std::cout << prefix << c.r << "," << c.g << "," << c.b << std::endl;
};
