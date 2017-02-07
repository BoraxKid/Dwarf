#ifndef DWARF_TEXTURE_H_
#define DWARF_TEXTURE_H_
#pragma once

#include <vulkan/vulkan.hpp>

#include "Tools.h"

namespace Dwarf
{
	class Texture
	{
	public:
		Texture(const std::string &texturePath);
		virtual ~Texture();

	private:
		const std::string _texturePath;

        uint32_t _width;
		uint32_t _height;
		uint32_t _mipLevels;
		uint32_t _layerCount;
	};
}

#endif // DWARF_TEXTURE_H_
