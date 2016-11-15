#ifndef MODEL_H_
#define MODEL_H_
#pragma once

#include "Mesh.h"

namespace Dwarf
{
	class Model
	{
	public:
		Model(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename);
		virtual ~Model();

		void init(vk::PhysicalDeviceMemoryProperties memProperties);

		void createBuffers(vk::PhysicalDeviceMemoryProperties memProperties);
		void buildCommandBuffers(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp);
        std::vector<vk::CommandBuffer> getCommandBuffers();

	private:
		Dwarf::Mesh _mesh;

		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;
	};
}

#endif // MODEL_H_
