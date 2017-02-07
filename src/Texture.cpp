#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Dwarf
{
	Texture::Texture(const std::string &texturePath)
        : _texturePath(texturePath)
	{
	}

	Texture::~Texture()
	{
	}
}
