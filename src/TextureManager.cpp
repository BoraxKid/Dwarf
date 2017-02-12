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
    }

    size_t TextureManager::addTexture(const std::string &textureName)
    {
        this->_textureNames[++this->_textureLastID] = textureName;
        return (this->_textureLastID);
    }

    void TextureManager::createTextures(AllocationManager &allocationManager)
    {
        int textureWidth;
        int textureHeight;
        int textureChannels;
        stbi_uc *pixels;

        for (const auto &textureName : this->_textureNames)
        {
            pixels = stbi_load(std::string("resources/textures/" + textureName.second).c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
            if (!pixels)
                Tools::exitOnError("Failed to load texture " + textureName.second + " file");
            vk::DeviceSize imageSize = textureWidth * textureHeight * STBI_rgb_alpha;
            this->_descriptorImageInfos[textureName.first] = allocationManager.createImage(pixels, imageSize, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
            stbi_image_free(pixels);
        }
    }

    const vk::DescriptorImageInfo &TextureManager::getDescriptorImageInfo(const size_t textureID) const
    {
        return (this->_descriptorImageInfos.at(textureID));
    }
}
