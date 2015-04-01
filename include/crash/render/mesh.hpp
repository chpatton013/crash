#pragma once

#include <map>
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

namespace crash {
namespace render {

class MatrixStack;
class ShaderProgram;

class Mesh {
public:
   Mesh(const Mesh& mesh);
   Mesh(const boost::filesystem::path& path);
   virtual ~Mesh();

   void initialize();
   void teardown();

   void bindAttributes(const ShaderProgram& program) const;
   void render(const ShaderProgram& program, MatrixStack& matrixStack) const;

   struct SceneImportFailure {
      SceneImportFailure(const std::string& error);
      std::string error;
   };

private:
   struct Component {
      Component(const Component& component);
      Component(const aiMesh* mesh, const GLuint& vao, const GLuint& vbo,
       const GLuint& ibo);

      void generateVertexArray();
      void generateVertexBuffer();
      void generateIndexBuffer();

      void bindAttributes(const ShaderProgram& program) const;
      void render(const ShaderProgram& program,
       const glm::mat4& transform) const;

      const aiMesh* mesh;
      GLuint vao;
      GLuint vbo;
      GLuint ibo;

      struct Vertex {
         Vertex(const Vertex& v);
         Vertex(const glm::vec3& position, const glm::vec3& normal,
          const glm::vec3& tangent, const glm::vec3& bitangent,
          const glm::vec3& textureCoordinates);
         Vertex(const aiVector3D& position, const aiVector3D& normal,
          const aiVector3D& tangent, const aiVector3D& bitangent,
          const aiVector3D& textureCoordinates);

         glm::vec3 position;
         glm::vec3 normal;
         glm::vec3 tangent;
         glm::vec3 bitangent;
         glm::vec3 textureCoordinates;

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

   void buildComponents();
   void destroyComponents();

   void allocateBuffers();
   void releaseBuffers();

   void renderNode(const ShaderProgram& program, MatrixStack& matrixStack,
    const aiNode* node) const;

   boost::filesystem::path _path;
   const aiScene* _scene;
   std::vector< GLuint > _vaos;
   std::vector< GLuint > _vbos;
   std::vector< GLuint > _ibos;
   std::map< const aiMesh*, Component > _components;
};

} // namespace render
} // namespace crash
