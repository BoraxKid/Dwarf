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
		MaterialManager(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue);
		virtual ~MaterialManager();
		bool exist(const Material::ID materialID) const;
		void addMaterial(Material *material);
        Material *createMaterial(const std::string &materialName);

	private:
        void createDescriptorSetLayout();
        void createPipelineLayout();
		bool isSame(const Material::ID &leftMaterialID, Material *rightMaterial) const;

        const vk::Device &_device;
        const vk::CommandPool &_commandPool;
        const vk::Queue &_graphicsQueue;
        vk::DescriptorSetLayout _descriptorSetLayout;
        vk::PipelineLayout _pipelineLayout;
        Material::ID _lastID;
        std::map<const std::string, Material::ID> _materialsNames;
        std::map<const Material::ID, Material *> _materials;
	};
}

#endif // MATERIALMANAGER_H_
