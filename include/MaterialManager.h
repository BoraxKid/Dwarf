#ifndef DWARF_MATERIALMANAGER_H_
#define DWARF_MATERIALMANAGER_H_
#pragma once

#include <vector>
#include <memory>
#include <assimp/scene.h>

#include "Material.h"
#include "Tools.h"

namespace Dwarf
{
	class MaterialManager
	{
	public:
		MaterialManager();
		virtual ~MaterialManager();

        size_t createMaterial(const aiMaterial *material);

	private:
        std::vector<std::unique_ptr<Material>> _materials;
	};
}

#endif // DWARF_MATERIALMANAGER_H_
