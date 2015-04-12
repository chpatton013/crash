#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <crash/render/texture.hpp>

using namespace crash::render;

Texture::ImportFailure::ImportFailure(const std::string& error) :
   error(error)
{}

Texture::Texture(const Texture& texture) :
   _path(texture._path), _data(texture._data),
    _width(texture._width), _height(texture._height),
    _components(texture._components)
{}

Texture::Texture(const boost::filesystem::path& path) :
   _path(path)
{
   this->importTexture();
}

/* virtual */ Texture::~Texture() {}

std::vector< unsigned char > Texture::getData() const {
   return this->_data;
}

int Texture::getWidth() const {
   return this->_width;
}

int Texture::getHeight() const {
   return this->_height;
}

int Texture::getComponents() const {
   return this->_components;
}

void Texture::importTexture() {
   unsigned char* data = stbi_load(this->_path.string().c_str(),
    &this->_width, &this->_height, &this->_components, 0);
   if (data == nullptr) {
      throw ImportFailure(this->_path.string());
   }

   int dataSize = this->_width * this->_height;
   this->_data.reserve(dataSize);
   ::memcpy(data, this->_data.data(), sizeof(unsigned char) * dataSize);

   stbi_image_free(data);
}
