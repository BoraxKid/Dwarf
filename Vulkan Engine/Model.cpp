#include "Model.h"

namespace Dwarf
{
	Model::Model(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename)
		: _mesh(device, commandPool, graphicsQueue, materialManager, meshFilename), _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue)
	{
	}

	Model::~Model()
	{
	}

	void Model::init(vk::PhysicalDeviceMemoryProperties memProperties)
	{
		this->createBuffers(memProperties);
	}

	void Model::createBuffers(vk::PhysicalDeviceMemoryProperties memProperties)
	{
		this->_mesh.createBuffers(memProperties);
	}

	void Model::buildCommandBuffers(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp)
	{
        this->_mesh.buildCommandBuffers(inheritanceInfo, mvp);
	}

	std::vector<vk::CommandBuffer> Model::getCommandBuffers()
	{
		return (this->_mesh.getCommandBuffers());
	}
}
