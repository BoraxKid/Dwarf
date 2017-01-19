#ifndef DWARF_SUBMESH_H_
#define DWARF_SUBMESH_H_
#pragma once

#include "Tools.h"
#include "IBuildable.h"
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
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv))
            };
            return (attributeDescriptions);
        }

        bool operator==(const Vertex &rhs) const
        {
            return (pos == rhs.pos && normal == rhs.normal && uv == rhs.uv);
        }

        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class Submesh : public IBuildable
    {
    public:
        Submesh(Material *material, const glm::mat4 &transformMatrix, const vk::DescriptorBufferInfo &lightBufferInfo);
        virtual ~Submesh();
        void cleanup(const vk::Device &device) const;
        virtual void createBuffers(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &memProperties);
        virtual void buildCommandBuffer(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp, const vk::Extent2D &extent) const;
        virtual void setCommandPool(vk::CommandPool *commandPool);
        virtual void setCommandBuffer(vk::CommandBuffer commandBuffer);
        void setVertices(const std::vector<Vertex> &vertices);
        void setIndices(const std::vector<uint32_t> &indices);
        size_t getVerticesCount() const;
        const std::vector<Vertex> &getVertices() const;
        size_t getIndicesCount() const;
        const std::vector<uint32_t> &getIndices() const;
        virtual vk::CommandBuffer getCommandBuffer() const;

    private:
        const vk::DescriptorBufferInfo &_lightBufferInfo;
        Material *_material;
        const glm::mat4 &_transform;
        vk::CommandPool *_commandPool;
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
                (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.uv) << 1));
        }
    };
}

#endif // DWARF_SUBMESH_H_
