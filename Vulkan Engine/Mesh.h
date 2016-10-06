#ifndef MESH_H_
#define MESH_H_
#pragma once

#include <unordered_map>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "Tools.h"
#include "Material.h"

namespace Dwarf
{
	class Renderer;

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Vertex
	{
	public:
		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return (vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex));
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions =
			{
				vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
				vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
				vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv))
			};
			return (attributeDescriptions);
		}

		bool operator==(const Vertex &rhs) const
		{
			return (pos == rhs.pos && color == rhs.color && uv == rhs.uv);
		}

		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 uv;
	};

	class Mesh
	{
	public:
		Mesh(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string &meshFilename);
		virtual ~Mesh();

		void loadFromFile(const std::string &filename);
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

		std::vector<Material *> _materials;
		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		vk::DeviceMemory _buffersMemory;
		vk::Buffer _buffer;
		vk::DeviceSize _vertexBufferOffset;
		vk::DeviceSize _indexBufferOffset;
		vk::DeviceSize _uniformBufferOffset;
	};
}

template<> struct std::hash<Dwarf::Vertex>
{
	size_t operator()(const Dwarf::Vertex &vertex) const
	{
		return (((std::hash<glm::vec3>()(vertex.pos) ^
			(std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			(std::hash<glm::vec2>()(vertex.uv) << 1));
	}
};

#endif // MESH_H_
