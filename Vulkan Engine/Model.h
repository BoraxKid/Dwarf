#ifndef MODEL_H_
#define MODEL_H_
#pragma once

#include "Mesh.h"

namespace Dwarf
{
	class Model
	{
	public:
		Model(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename, const std::string &textureFilename);
		virtual ~Model();

		void init(vk::PhysicalDeviceMemoryProperties memProperties, vk::DescriptorPool &descriptorPool, vk::DescriptorSetLayout &descriptorSetLayout);

		void createBuffers(vk::PhysicalDeviceMemoryProperties memProperties);
		void buildCommandBuffer(vk::CommandBufferInheritanceInfo &inheritanceInfo, vk::Pipeline &graphicsPipeline, vk::PipelineLayout &pipelineLayout, glm::mat4 mvp);
		vk::CommandBuffer getCommandBuffer();

	private:
		Dwarf::Mesh _mesh;
		Dwarf::Texture *_texture;

		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;

		vk::DescriptorSet _descriptorSet;

		vk::CommandBuffer _commandBuffer;
	};
}

#endif // MODEL_H_
