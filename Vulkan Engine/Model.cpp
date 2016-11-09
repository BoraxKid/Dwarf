#include "Model.h"

namespace Dwarf
{
	Model::Model(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename, const std::string &textureFilename)
		: _mesh(device, commandPool, graphicsQueue, materialManager, meshFilename), _texture(nullptr), _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue)
	{
		if (!textureFilename.empty())
			this->_texture = new Texture(device, commandPool, graphicsQueue, textureFilename);
	}

	Model::~Model()
	{
		if (this->_texture)
			delete (this->_texture);
		this->_device.freeCommandBuffers(this->_commandPool, this->_commandBuffer);
	}

	void Model::init(vk::PhysicalDeviceMemoryProperties memProperties, vk::DescriptorPool &descriptorPool, vk::DescriptorSetLayout &descriptorSetLayout)
	{
		this->createBuffers(memProperties);
		if (this->_texture)
		{
			vk::DescriptorImageInfo &imageInfo = this->_texture->createTexture(memProperties, descriptorPool, descriptorSetLayout);

			vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &descriptorSetLayout);
			this->_descriptorSet = this->_device.allocateDescriptorSets(allocInfo).at(0);
			vk::DescriptorBufferInfo bufferInfo(this->_mesh.getBuffer(), this->_mesh.getUniformBufferOffset(), sizeof(UniformBufferObject));
			std::array<vk::WriteDescriptorSet, 2> descriptorWrites = { vk::WriteDescriptorSet(this->_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo), vk::WriteDescriptorSet(this->_descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo) };
			this->_device.updateDescriptorSets(descriptorWrites, nullptr);
		}
	}

	void Model::createBuffers(vk::PhysicalDeviceMemoryProperties memProperties)
	{
		this->_mesh.createBuffers(memProperties);
		vk::CommandBufferAllocateInfo cmbBufferAllocInfo(this->_commandPool, vk::CommandBufferLevel::eSecondary, 1);
		this->_commandBuffer = this->_device.allocateCommandBuffers(cmbBufferAllocInfo).at(0);
	}

	void Model::buildCommandBuffer(vk::CommandBufferInheritanceInfo &inheritanceInfo, vk::Pipeline &graphicsPipeline, vk::PipelineLayout &pipelineLayout, glm::mat4 mvp)
	{
		vk::Buffer buffer = this->_mesh.getBuffer();
		this->_commandBuffer.reset(vk::CommandBufferResetFlags());
		vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse, &inheritanceInfo);
		this->_commandBuffer.begin(beginInfo);
		this->_commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f));
		this->_commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(1280, 720)));
		this->_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		this->_commandBuffer.pushConstants<glm::mat4>(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, mvp);

		this->_commandBuffer.bindVertexBuffers(0, buffer, this->_mesh.getVertexBufferOffset());
		this->_commandBuffer.bindIndexBuffer(buffer, this->_mesh.getIndexBufferOffset(), vk::IndexType::eUint32);
		this->_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, this->_descriptorSet, nullptr);
		this->_commandBuffer.drawIndexed(static_cast<uint32_t>(this->_mesh.getIndicesCount()), 1, 0, 0, 0);
		this->_commandBuffer.end();
	}

	vk::CommandBuffer Model::getCommandBuffer()
	{
		return (this->_commandBuffer);
	}
}
