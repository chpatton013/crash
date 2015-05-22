#pragma once

#include <exception>
#include <vector>
#include <boost/filesystem/path.hpp>

namespace crash {
namespace render {

class Texture;
typedef std::shared_ptr< Texture > TexturePtr;

class Texture {
public:
   class ImportFailure : public std::exception {
   public:
      ImportFailure(const std::string& error);
      const char* what() const noexcept;

   private:
      std::string _error;
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
   TextureGroup(const TexturePtr& displacement, const TexturePtr& normal,
    const TexturePtr& ambient, const TexturePtr& diffuse,
    const TexturePtr& specular, const TexturePtr& shininess);
   TexturePtr displacement;
   TexturePtr normal;
   TexturePtr ambient;
   TexturePtr diffuse;
   TexturePtr specular;
   TexturePtr shininess;
};

} // namespace render
} // namespace crash
