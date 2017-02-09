#ifndef DWARF_MATERIALMANAGER_H_
#define DWARF_MATERIALMANAGER_H_
#pragma once

#include <vector>
#include <memory>
#include <assimp/scene.h>

#include "Material.h"
#include "AllocationManager.h"
#include "Tools.h"
#include "ModelData.h"
#include "TextureManager.h"

namespace Dwarf
{
    struct ModelVulkanData;

    class MaterialManager
	{
	public:
		MaterialManager();
		virtual ~MaterialManager();

        size_t addMaterial(const aiMaterial *material);
        void createMaterials(AllocationManager &allocationManager, const std::vector<ModelData> &modelDatas, std::vector<ModelVulkanData> &modelVulkanDatas);

	private:
        TextureManager _textureManager;
        std::vector<std::unique_ptr<Material>> _materials;
	};
}

#endif // DWARF_MATERIALMANAGER_H_
