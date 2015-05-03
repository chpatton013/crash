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

#include <crash/math/transformer.hpp>
#include <crash/math/transformable.hpp>
#include <crash/render/animation.hpp>
#include <crash/render/mesh_component.hpp>
#include <crash/render/texture.hpp>

namespace crash {
namespace render {

class MatrixStack;
class ShaderProgram;
struct AttributeVariable;
struct UniformVariable;

struct AnimationUnit {
   AnimationUnit(unsigned int index, float duration);
   unsigned int index;
   float duration;
};

class Mesh : public math::Transformable {
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
   // Transformable interface.
   /////////////////////////////////////////////////////////////////////////////

   const glm::vec3& getPosition() const;
   const glm::vec4& getOrientation() const;
   const glm::vec3& getSize() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::vec4& orientation);
   void setSize(const glm::vec3& size);

   const glm::mat4& getTransform();

   /////////////////////////////////////////////////////////////////////////////
   // Rendering.
   /////////////////////////////////////////////////////////////////////////////

   const std::vector< Animation >& getAnimations() const;

   void initialize();
   void teardown();

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;

   void render(const ShaderProgram& program, const UniformVariable& vars,
    MatrixStack& matrixStack,
    const std::vector< AnimationUnit >& activeAnimations);

private:
   void importScene();
   void releaseScene();

   void importTextures();
   std::shared_ptr< Texture > importTexture(const aiMaterial* material,
    const aiTextureType& type, unsigned int index);
   void normalizeScene();
   void buildAnimations();

   void buildComponents();
   void destroyComponents();

   void allocateBuffers();
   void releaseBuffers();

   void renderNode(const ShaderProgram& program, const UniformVariable& sig,
     MatrixStack& matrixStack, const aiNode* node,
     const std::vector< AnimationUnit >& activeAnimations) const;
   glm::mat4 getNodeTransform(const aiNode* node,
    const std::vector< AnimationUnit >& activeAnimations) const;

   boost::filesystem::path _path;
   const aiScene* _scene;
   std::vector< Animation > _animations;
   math::Transformer _transformer;
   std::vector< GLuint > _vaos;
   std::vector< GLuint > _vbos;
   std::vector< GLuint > _ibos;
   std::vector< GLuint > _tbos;
   std::map< const aiMesh*, MeshComponent > _components;
   std::vector< TextureGroup > _textureGroups;
};

} // namespace render
} // namespace crash
