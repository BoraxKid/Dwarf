#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_
#pragma once

#include <map>

#include "Material.h"

namespace Dwarf
{
	class MaterialManager
	{
	public:
		MaterialManager(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::RenderPass &renderPass, const vk::Extent2D &swapChainExtent);
		virtual ~MaterialManager();
        bool exist(const Material::ID materialID) const;
        bool exist(const std::string &materialName) const;
		void addMaterial(Material *material);
        Material *getMaterial(const std::string &materialName) const;
        Material *createMaterial(const std::string &materialName);

	private:
        void createDescriptorSetLayout();
        void createPipelineLayout();
        void createMaterialPipeline(const Material::ID materialID);
		bool isSame(const Material::ID &leftMaterialID, Material *rightMaterial) const;

        const vk::Device &_device;
        const vk::CommandPool &_commandPool;
        const vk::Queue &_graphicsQueue;
        const vk::RenderPass &_renderPass;
        const vk::Extent2D &_swapChainExtent;
        vk::DescriptorSetLayout _descriptorSetLayout;
        vk::PipelineLayout _pipelineLayout;
        Material::ID _lastID;
        std::map<const std::string, Material::ID> _materialsNames;
        std::map<const Material::ID, Material *> _materials;
        std::map<const Material::ID, vk::Pipeline> _pipelines;
	};
}

#endif // MATERIALMANAGER_H_
