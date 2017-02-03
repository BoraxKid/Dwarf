#ifndef DWARF_SUBMESH_H_
#define DWARF_SUBMESH_H_
#pragma once

#include "Tools.h"
#include "IBuildable.h"
#include "Material.h"
#include "MeshData.h"

namespace Dwarf
{
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
        void setBuffer(const vk::Buffer &buffer);
        void setVertexBufferOffset(const vk::DeviceSize &vertexBufferOffset);
        void setIndexBufferOffset(const vk::DeviceSize &indexBufferOffset);
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
        vk::Buffer _uniformBuffer;
        vk::DeviceSize _vertexBufferOffset;
        vk::DeviceSize _indexBufferOffset;
        vk::DeviceSize _uniformBufferOffset;
        vk::CommandBuffer _commandBuffer;
    };
}

#endif // DWARF_SUBMESH_H_
