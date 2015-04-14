#pragma once

#include <vector>
#include <boost/filesystem/path.hpp>

namespace crash {
namespace render {

class Texture {
public:
   struct ImportFailure {
      ImportFailure(const std::string& error);
      std::string error;
   };

   Texture(const Texture& texture);
   Texture(const boost::filesystem::path& path);
   virtual ~Texture();

   const boost::filesystem::path& getPath() const;
   const std::vector< float >& getData() const;
   int getWidth() const;
   int getHeight() const;
   int getComponents() const;

private:
   void importTexture();

   boost::filesystem::path _path;
   std::vector< float > _data;
   int _width;
   int _height;
   int _components;
};

struct TextureGroup {
   TextureGroup(
    const std::shared_ptr< Texture >& displacement,
    const std::shared_ptr< Texture >& normal,
    const std::shared_ptr< Texture >& ambient,
    const std::shared_ptr< Texture >& diffuse,
    const std::shared_ptr< Texture >& specular,
    const std::shared_ptr< Texture >& shininess);
   std::shared_ptr< Texture > displacement;
   std::shared_ptr< Texture > normal;
   std::shared_ptr< Texture > ambient;
   std::shared_ptr< Texture > diffuse;
   std::shared_ptr< Texture > specular;
   std::shared_ptr< Texture > shininess;
};

} // namespace render
} // namespace crash
