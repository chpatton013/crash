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
#include <crash/render/texture.hpp>
#include <crash/render/mesh_component.hpp>

namespace crash {
namespace render {

class MatrixStack;
class ShaderProgram;
struct AttributeVariable;
struct UniformVariable;

class Mesh : public math::Transformable {
public:
   struct SceneImportFailure {
      SceneImportFailure(const std::string& error);
      std::string error;
   };

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

   void initialize();
   void teardown();

   void bindAttributes(const ShaderProgram& program,
    const AttributeVariable& vars) const;

   void render(const ShaderProgram& program, const UniformVariable& vars,
    MatrixStack& matrixStack);

private:
   void importScene();
   void releaseScene();

   void importTextures();
   void normalizeScene();

   void buildComponents();
   void destroyComponents();

   void allocateBuffers();
   void releaseBuffers();

   void renderNode(const ShaderProgram& program, const UniformVariable& sig,
     MatrixStack& matrixStack, const aiNode* node) const;

   boost::filesystem::path _path;
   const aiScene* _scene;
   math::Transformer _transformer;
   std::vector< GLuint > _vaos;
   std::vector< GLuint > _vbos;
   std::vector< GLuint > _ibos;
   std::vector< GLuint > _tbos;
   std::map< const aiMesh*, MeshComponent > _components;
   std::vector< std::shared_ptr< Texture > > _textures;
};

} // namespace render
} // namespace crash
