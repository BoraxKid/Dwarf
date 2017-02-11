#ifndef DWARF_TEXTUREMANAGER_H_
#define DWARF_TEXTUREMANAGER_H_
#pragma once

#include "AllocationManager.h"

namespace Dwarf
{
    struct Texture
    {
        Texture()
            : imageDataID(0)
        {}

        Texture(const std::string &name)
            : name(name), imageDataID(0)
        {}

        virtual ~Texture()
        {}

        std::string name;
        size_t imageDataID;
    };

    class TextureManager
    {
    public:
        TextureManager();
        virtual ~TextureManager();

        size_t addTexture(const std::string &textureName);
        void createTextures(AllocationManager &allocationManager);

    private:
        std::map<const size_t, Texture> _textures;
        size_t _textureLastID;
    };
}

#endif // DWARF_TEXTUREMANAGER_H_
