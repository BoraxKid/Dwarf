#ifndef DWARF_MATERIALMANAGER_H_
#define DWARF_MATERIALMANAGER_H_
#pragma once

#include <map>

#include "Material.h"
#include "LightManager.h"

namespace Dwarf
{
	class MaterialManager
	{
	public:
		MaterialManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::RenderPass &renderPass, const vk::Extent2D &swapChainExtent, LightManager &lightManager);
		virtual ~MaterialManager();
        bool exist(const Material::ID materialID) const;
        bool exist(const std::string &materialName) const;
		void addMaterial(Material *material);
        Material *getMaterial(const std::string &materialName) const;
        Material *createMaterial(const std::string &materialName, bool diffuseTexture);
        void createDescriptorPool();
        void recreatePipelines();

	private:
        void createDescriptorSetLayout();
        void createPipelineLayout(const vk::DescriptorSetLayout &lightDescriptorSetLayout);
        void createMaterialPipeline(const Material::ID &materialID, bool diffuseTexture);
		bool isSame(const Material::ID &leftMaterialID, Material *rightMaterial) const;

        const vk::Device &_device;
        const vk::Queue &_graphicsQueue;
        const vk::RenderPass &_renderPass;
        const vk::Extent2D &_swapChainExtent;
        vk::DescriptorSetLayout _descriptorSetLayout;
        vk::PipelineLayout _pipelineLayout;
        vk::DescriptorPool _descriptorPool;
        Material::ID _lastID;
        std::map<const std::string, Material::ID> _materialsNames;
        std::map<const Material::ID, Material *> _materials;
        std::map<const Material::ID, vk::Pipeline> _pipelines;
        std::map<const Material::ID, vk::DescriptorSet> _descriptorSets;
	};
}

#endif // DWARF_MATERIALMANAGER_H_
