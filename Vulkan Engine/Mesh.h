#ifndef MESH_H_
#define MESH_H_
#pragma once

#include <unordered_map>
#include <chrono>

#include "Tools.h"
#include "Submesh.h"
#include "MaterialManager.h"

namespace Dwarf
{
	class Renderer;

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Mesh
	{
	public:
		Mesh(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename);
		virtual ~Mesh();

		void loadFromFile(Dwarf::MaterialManager &materialManager, const std::string &filename);
		void createBuffers(vk::PhysicalDeviceMemoryProperties memProperties);

		vk::Buffer getBuffer() const;
		size_t getIndicesCount() const;
		vk::DeviceSize getVertexBufferOffset() const;
		vk::DeviceSize getIndexBufferOffset() const;
		vk::DeviceSize getUniformBufferOffset() const;

	private:
		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;

        std::vector<Submesh> _submeshes;

		vk::DeviceMemory _buffersMemory;
		vk::Buffer _buffer;
		vk::DeviceSize _vertexBufferOffset;
		vk::DeviceSize _indexBufferOffset;
		vk::DeviceSize _uniformBufferOffset;
	};
}

#endif // MESH_H_
