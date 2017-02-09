#ifndef DWARF_TEXTUREMANAGER_H_
#define DWARF_TEXTUREMANAGER_H_
#pragma once

#include "Texture.h"
#include "ImageManager.h"
#include "AllocationManager.h"

namespace Dwarf
{
    class TextureManager
    {
    public:
        TextureManager();
        virtual ~TextureManager();

        size_t addTexture(const std::string &textureName);
        void createTextures(AllocationManager &allocationManager);

    private:
        ImageManager _imageManager;
        std::vector<std::unique_ptr<Texture>> _textures;
    };
}

#endif // DWARF_TEXTUREMANAGER_H_
