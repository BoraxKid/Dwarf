#ifndef SUBMESH_H_
#define SUBMESH_H_
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "Tools.h"
#include "Material.h"

namespace Dwarf
{
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

    class Submesh
    {
    public:
        Submesh(Material *material);
        virtual ~Submesh();
        void cleanup(const vk::Device &device);
        void createBuffers(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, vk::PhysicalDeviceMemoryProperties memProperties);
        void buildCommandBuffer(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp);
        void setVertices(const std::vector<Vertex> &vertices);
        void setIndices(const std::vector<uint32_t> &indices);
        void setCommandBuffer(vk::CommandBuffer commandBuffer);
        vk::CommandBuffer getCommandBuffer() const;

    private:
        Material *_material;
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
        vk::DeviceMemory _buffersMemory;
        vk::Buffer _buffer;
        vk::DeviceSize _vertexBufferOffset;
        vk::DeviceSize _indexBufferOffset;
        vk::DeviceSize _uniformBufferOffset;
        vk::CommandBuffer _commandBuffer;
    };
}

namespace std
{
    template<> struct hash<Dwarf::Vertex>
    {
        size_t operator()(const Dwarf::Vertex &vertex) const
        {
            return (((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.uv) << 1));
        }
    };
}

#endif // SUBMESH_H_
