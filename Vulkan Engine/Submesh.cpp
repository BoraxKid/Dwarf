#include "Submesh.h"

namespace Dwarf
{
    Submesh::Submesh(Material *material)
        : _material(material)
    {
    }

    Submesh::~Submesh()
    {
    }

    void Submesh::setVertices(const std::vector<Vertex> &vertices)
    {
        this->_vertices = vertices;
    }

    void Submesh::setIndices(const std::vector<uint32_t> &indices)
    {
        this->_indices = indices;
    }

    void Submesh::createBuffers(vk::PhysicalDeviceMemoryProperties memProperties)
    {
        vk::DeviceSize vertexBufferSize = sizeof(Vertex) * this->_vertices.size();
        vk::DeviceSize indexBufferSize = sizeof(uint32_t) * this->_indices.size();
        vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer);
        vk::Buffer vertexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        bufferInfo.setSize(indexBufferSize);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
        vk::Buffer indexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);


        vk::MemoryRequirements memRequirements[2];
        memRequirements[0] = this->_device.getBufferMemoryRequirements(vertexBuffer);
        memRequirements[1] = this->_device.getBufferMemoryRequirements(indexBuffer);
        this->_vertexBufferOffset = 0;
        this->_indexBufferOffset = memRequirements[0].size + (memRequirements[1].alignment - (memRequirements[0].size % memRequirements[1].alignment));
        bufferInfo.setSize(this->_indexBufferOffset + memRequirements[1].size);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);

        vk::Buffer stagingBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        uint32_t memTypes = memRequirements[0].memoryTypeBits & memRequirements[1].memoryTypeBits;

        vk::MemoryAllocateInfo allocInfo(bufferInfo.size, Tools::getMemoryType(memProperties, memTypes, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        vk::DeviceMemory stagingBufferMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);

        this->_device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
        void *data = this->_device.mapMemory(stagingBufferMemory, this->_vertexBufferOffset, memRequirements[0].size);
        memcpy(data, this->_vertices.data(), static_cast<size_t>(vertexBufferSize));
        this->_device.unmapMemory(stagingBufferMemory);

        data = this->_device.mapMemory(stagingBufferMemory, this->_indexBufferOffset, memRequirements[1].size);
        memcpy(data, this->_indices.data(), static_cast<size_t>(indexBufferSize));
        this->_device.unmapMemory(stagingBufferMemory);

        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), this->_indexBufferOffset + indexBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer);
        this->_buffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memRequirements[0] = this->_device.getBufferMemoryRequirements(this->_buffer);
        allocInfo = vk::MemoryAllocateInfo(memRequirements[0].size, Tools::getMemoryType(memProperties, memRequirements[0].memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
        if (this->_buffersMemory != (vk::DeviceMemory)VK_NULL_HANDLE)
            this->_device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
        this->_buffersMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
        this->_device.bindBufferMemory(this->_buffer, this->_buffersMemory, 0);
        vk::CommandBuffer commandBuffer = Tools::beginSingleTimeCommands(this->_device, this->_commandPool);
        commandBuffer.copyBuffer(stagingBuffer, this->_buffer, vk::BufferCopy(0, 0, this->_indexBufferOffset + indexBufferSize));
        Tools::endSingleTimeCommands(this->_device, this->_graphicsQueue, this->_commandPool, commandBuffer);

        this->_device.freeMemory(stagingBufferMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(stagingBuffer, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(indexBuffer, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(vertexBuffer, CUSTOM_ALLOCATOR);
    }
}
