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

namespace crash {
namespace render {

class MatrixStack;
class ShaderProgram;
struct UniformVariable;

class Mesh : public math::Transformable {
public:
   struct SceneImportFailure {
      SceneImportFailure(const std::string& error);
      std::string error;
   };

   static const glm::vec4 defaultAmbientColor;
   static const glm::vec4 defaultDiffuseColor;
   static const glm::vec4 defaultSpecularColor;
   static const float defaultShininess;

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

   void bindAttributes(const ShaderProgram& program) const;
   void render(const ShaderProgram& program, const UniformVariable& vars,
    MatrixStack& matrixStack);

private:
   struct Component {
      Component(const Component& component);
      Component(const aiMesh* mesh, const aiMaterial* material,
       std::shared_ptr< Texture > texture, const GLuint& vao,
       const GLuint& vbo, const GLuint& ibo, const GLuint& tbo);

      void generateVertexArray();
      void generateVertexBuffer();
      void generateIndexBuffer();
      void generateTextureBuffer();

      void bindAttributes(const ShaderProgram& program) const;
      void render(const ShaderProgram& program, const UniformVariable& vars,
       const glm::mat4& transform) const;
      void setMaterialProperties();

      const aiMesh* mesh;
      const aiMaterial* material;
      glm::vec4 ambient;
      glm::vec4 diffuse;
      glm::vec4 specular;
      float shininess;
      bool twoSided;
      std::shared_ptr< Texture > texture;
      GLuint vao;
      GLuint vbo;
      GLuint ibo;
      GLuint tbo;

      struct Vertex {
         Vertex(const Vertex& v);
         Vertex(const glm::vec3& position, const glm::vec3& normal,
          const glm::vec3& tangent, const glm::vec3& bitangent,
          const glm::vec2& textureCoordinates);
         Vertex(const aiVector3D& position, const aiVector3D& normal,
          const aiVector3D& tangent, const aiVector3D& bitangent,
          const aiVector2D& textureCoordinates);

         glm::vec3 position;
         glm::vec3 normal;
         glm::vec3 tangent;
         glm::vec3 bitangent;
         glm::vec2 textureCoordinates;

         struct Attribute {
            Attribute(unsigned int index, const std::string& name,
             size_t offset, size_t width);

            unsigned int index;
            std::string name;
            size_t offset;
            size_t width;
         };

         static std::vector< Attribute > attributes;
      };
   };

   void importScene();
   void releaseScene();

   void importTextures();
   void normalizeScene();

   void buildComponents();
   void destroyComponents();

   void allocateBuffers();
   void releaseBuffers();

   void renderNode(const ShaderProgram& program, const UniformVariable& vars,
     MatrixStack& matrixStack, const aiNode* node) const;

   boost::filesystem::path _path;
   const aiScene* _scene;
   math::Transformer _transformer;
   std::vector< GLuint > _vaos;
   std::vector< GLuint > _vbos;
   std::vector< GLuint > _ibos;
   std::vector< GLuint > _tbos;
   std::map< const aiMesh*, Component > _components;
   std::vector< std::shared_ptr< Texture > > _textures;
};

} // namespace render
} // namespace crash
