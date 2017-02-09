#include "TextureManager.h"

namespace Dwarf
{
    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
    }

    size_t TextureManager::addTexture(const std::string &textureName)
    {
        this->_textures.push_back(std::make_unique<Texture>(this->_textures.size, textureName));
        return (this->_textures.back()->getID());
    }

    void TextureManager::createTextures(AllocationManager &allocationManager)
    {
    }
}
