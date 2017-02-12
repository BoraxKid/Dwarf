#ifndef DWARF_TEXTUREMANAGER_H_
#define DWARF_TEXTUREMANAGER_H_
#pragma once

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
        const vk::DescriptorImageInfo &getDescriptorImageInfo(const size_t textureID) const;

    private:
        std::map<const size_t, std::string> _textureNames;
        std::map<const size_t, vk::DescriptorImageInfo> _descriptorImageInfos;
        size_t _textureLastID;
    };
}

#endif // DWARF_TEXTUREMANAGER_H_
