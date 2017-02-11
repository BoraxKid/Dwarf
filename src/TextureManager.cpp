#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Dwarf
{
    TextureManager::TextureManager()
        : _textureLastID(0)
    {
    }

    TextureManager::~TextureManager()
    {
        this->_textures.clear();
    }

    size_t TextureManager::addTexture(const std::string &textureName)
    {
        this->_textures[++this->_textureLastID] = Texture(textureName);
        return (this->_textures.size() - 1);
    }

    void TextureManager::createTextures(AllocationManager &allocationManager)
    {
        int textureWidth;
        int textureHeight;
        int textureChannels;
        stbi_uc *pixels;

        for (auto &texture : this->_textures)
        {
            pixels = stbi_load(std::string("resources/textures/" + texture.second.name).c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
            if (!pixels)
                Tools::exitOnError("Failed to load texture " + texture.second.name + " file");
            vk::DeviceSize imageSize = textureWidth * textureHeight * STBI_rgb_alpha;
            texture.second.imageDataID = allocationManager.createImage(pixels, imageSize, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
            stbi_image_free(pixels);
        }
    }
}
