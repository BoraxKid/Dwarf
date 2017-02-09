#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Dwarf
{
    Texture::Texture(const size_t ID, const std::string &texturePath)
        : _ID(ID), _texturePath(texturePath)
	{
	}

	Texture::~Texture()
	{
	}

    void Texture::loadTexture(AllocationManager &allocationManager)
    {
        int textureWidth;
        int textureHeight;
        int textureChannels;
        stbi_uc *pixels = stbi_load(std::string("resources/textures/" + this->_texturePath).c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
        if (!pixels)
            Tools::exitOnError("Failed to load texture " + this->_texturePath + " file");
        this->_width = static_cast<uint32_t>(textureWidth);
        this->_height = static_cast<uint32_t>(textureHeight);
        vk::DeviceSize imageSize = this->_width * this->_height * STBI_rgb_alpha;

        allocationManager.createImage(pixels, imageSize, this->_width, this->_height);

        stbi_image_free(pixels);
    }

    const size_t &Texture::getID() const
    {
        return (this->_ID);
    }

    const std::string &Texture::getTexturePath() const
    {
        return (this->_texturePath);
    }
}
