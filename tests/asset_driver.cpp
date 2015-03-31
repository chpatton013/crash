#include <iostream>
#include <string>
#include <assimp/cimport.h>         // Plain-C interface
#include <assimp/postprocess.h>     // Post processing flags
#include <crash/render/util.hpp>

struct aiScene;

const aiScene* importFile(const std::string& file);

int main(int argc, char** argv) {
   if (argc < 2) {
      std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
      return 1;
   }
   const std::string file = argv[1];

   auto scene = importFile(file);
   if (scene == NULL) {
      return 2;
   }

   crash::render::printScene(scene, "");

   aiReleaseImport(scene);

   return 0;
}

const aiScene* importFile(const std::string& file) {
   static const unsigned int postProcessingFlags =
      aiProcessPreset_TargetRealtime_MaxQuality
    | aiProcess_FixInfacingNormals
    | aiProcess_OptimizeGraph
    | aiProcess_TransformUVCoords;

   auto scene = aiImportFile(file.data(), postProcessingFlags);

   if (scene == NULL) {
      std::cerr << aiGetErrorString() << std::endl;
      return NULL;
   }

   return scene;
}
