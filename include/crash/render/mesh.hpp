#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <boost/filesystem/path.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <boost/predef/os.h>
#ifdef BOOST_OS_MACOS
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <crash/common/movable.hpp>
#include <crash/common/transformer.hpp>
#include <crash/render/animation.hpp>
#include <crash/render/mesh_component.hpp>
#include <crash/render/texture.hpp>

namespace crash {
namespace render {

class ShaderProgram;
struct AttributeVariable;

typedef std::map< std::string, const aiNode* > NodeNameMap;

class Mesh;
typedef std::shared_ptr< Mesh > MeshPtr;

class Mesh : public common::Movable {
public:
   struct SceneImportFailure {
      SceneImportFailure(const std::string& error);
      std::string error;
   };

   static const unsigned int NUM_TEXTURE_TYPES;

   Mesh(const Mesh& mesh);
   Mesh(const boost::filesystem::path& path);
   virtual ~Mesh();

   /////////////////////////////////////////////////////////////////////////////
   // Movable interface.
   /////////////////////////////////////////////////////////////////////////////

   glm::vec3 getPosition() const;
   glm::quat getOrientation() const;
   glm::vec3 getSize() const;
   glm::vec3 getTranslationalVelocity() const;
   glm::quat getRotationalVelocity() const;
   glm::vec3 getScaleVelocity() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::quat& orientation);
   void setSize(const glm::vec3& size);
   void setTranslationalVelocity(const glm::vec3& translationalVelocity);
   void setRotationalVelocity(const glm::quat& rotationalVelocity);
   void setScaleVelocity(const glm::vec3& scaleVelocity);

   /////////////////////////////////////////////////////////////////////////////
   // Rendering.
   /////////////////////////////////////////////////////////////////////////////

   const aiScene* getScene() const;
   const std::vector< Animation >& getAnimations() const;

   void initialize();
   void teardown();

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;

   void render(const ShaderProgram& program, const ColorUnit& color,
    const glm::mat4& parentTransform,
    const NodeTransformMap& nodeTransforms) const;

private:
   /////////////////////////////////////////////////////////////////////////////
   // Allocation.
   /////////////////////////////////////////////////////////////////////////////

   const aiScene* importScene();
   void normalizeScene();
   void buildAnimations();
   void buildBoneNodeMap();
   void importTextures();

   /////////////////////////////////////////////////////////////////////////////
   // Graphics card export.
   /////////////////////////////////////////////////////////////////////////////

   void allocateBuffers();
   void buildComponents();

   /////////////////////////////////////////////////////////////////////////////
   // Deallocation.
   /////////////////////////////////////////////////////////////////////////////

   void releaseScene();
   void releaseBuffers();
   void destroyComponents();

   /////////////////////////////////////////////////////////////////////////////
   // Helpers.
   /////////////////////////////////////////////////////////////////////////////

   TexturePtr importTexture(const aiMaterial* material,
    const aiTextureType& type, unsigned int index);

   void buildComponentsHelper(const aiNode* node);
   void buildComponentHelper(const aiNode* node);

   NodeNameMap getNodeNameMap() const;
   void getNodeNameMapHelper(const aiNode* node, NodeNameMap& nodeNames) const;

   void renderNode(const ShaderProgram& program, const ColorUnit& color,
    const glm::mat4& globalTransform, const NodeTransformMap& nodeTransforms,
    const aiNode* node) const;

   /////////////////////////////////////////////////////////////////////////////
   // Members.
   /////////////////////////////////////////////////////////////////////////////

   boost::filesystem::path _path;
   const aiScene* _scene;
   common::Transformer _transformer;
   BoneNodeMap _boneNodes;
   std::vector< Animation > _animations;
   std::vector< TextureGroup > _textureGroups;
   std::vector< GLuint > _vaos;
   std::vector< GLuint > _vbos;
   std::vector< GLuint > _ibos;
   std::vector< GLuint > _tbos;
   std::map< const aiMesh*, MeshComponent > _components;
};

} // namespace render
} // namespace crash
