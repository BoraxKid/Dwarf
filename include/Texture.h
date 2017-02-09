#ifndef DWARF_TEXTURE_H_
#define DWARF_TEXTURE_H_
#pragma once

#include <vulkan/vulkan.hpp>

#include "AllocationManager.h"

namespace Dwarf
{
	class Texture
	{
	public:
		Texture(const size_t ID, const std::string &texturePath);
		virtual ~Texture();

        void loadTexture(AllocationManager &allocationManager);
        const size_t &getID() const;
        const std::string &getTexturePath() const;

	private:
        const size_t _ID;
		const std::string _texturePath;

        uint32_t _width;
		uint32_t _height;
		uint32_t _mipLevels;
		uint32_t _layerCount;
	};
}

#endif // DWARF_TEXTURE_H_
