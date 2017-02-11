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
		MaterialManager(const vk::Device &device);
		virtual ~MaterialManager();

        void init();
        size_t addMaterial(const aiMaterial *material);
        void createMaterials(AllocationManager &allocationManager, const std::vector<ModelData> &modelDatas, std::vector<ModelVulkanData> &modelVulkanDatas);
        void recreatePipelines(const vk::Extent2D &extent, const vk::RenderPass &renderPass);
        
    private:
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createPipelineLayout();
        void createMaterialPipeline(const vk::Extent2D &extent, const vk::RenderPass &renderPass, const size_t materialID, const bool texture);

        const vk::Device &_device;
        TextureManager _textureManager;
        vk::DescriptorSetLayout _descriptorSetLayout;
        vk::PipelineLayout _pipelineLayout;
        vk::DescriptorPool _descriptorPool;
        std::map<const size_t, Material> _materials;
        size_t _materialLastID;
        std::map<const size_t, vk::DescriptorSet> _descriptorSets;
        size_t _descriptorSetLastID;
        std::map<const size_t, vk::Pipeline> _pipelines;
        size_t _pipelineLastID;
	};
}

#endif // DWARF_MATERIALMANAGER_H_
