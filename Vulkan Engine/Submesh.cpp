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

    void Submesh::cleanup(const vk::Device &device)
    {
        device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
        device.destroyBuffer(this->_buffer, CUSTOM_ALLOCATOR);
    }

    void Submesh::createBuffers(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, vk::PhysicalDeviceMemoryProperties memProperties)
    {
        vk::DeviceSize vertexBufferSize = sizeof(Vertex) * this->_vertices.size();
        vk::DeviceSize indexBufferSize = sizeof(uint32_t) * this->_indices.size();
        vk::DeviceSize uniformBufferSize = sizeof(MaterialUniformBuffer);
        vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer);
        vk::Buffer vertexBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        bufferInfo.setSize(indexBufferSize);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
        vk::Buffer indexBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        bufferInfo.setSize(uniformBufferSize);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
        vk::Buffer uniformBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);

        vk::MemoryRequirements memRequirements[3];
        memRequirements[0] = device.getBufferMemoryRequirements(vertexBuffer);
        memRequirements[1] = device.getBufferMemoryRequirements(indexBuffer);
        memRequirements[2] = device.getBufferMemoryRequirements(uniformBuffer);

        this->_vertexBufferOffset = 0;
        this->_indexBufferOffset = memRequirements[0].size + (memRequirements[1].alignment - (memRequirements[0].size % memRequirements[1].alignment));
        this->_uniformBufferOffset = (this->_indexBufferOffset + memRequirements[1].size) + (memRequirements[2].size - ((this->_indexBufferOffset + memRequirements[1].size) % memRequirements[2].alignment));

        bufferInfo.setSize(this->_uniformBufferOffset + memRequirements[2].size);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);

        vk::Buffer stagingBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        uint32_t memTypes = memRequirements[0].memoryTypeBits & memRequirements[1].memoryTypeBits & memRequirements[2].memoryTypeBits;

        vk::MemoryAllocateInfo allocInfo(bufferInfo.size, Tools::getMemoryType(memProperties, memTypes, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        vk::DeviceMemory stagingBufferMemory = device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);

        device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
        void *data;
        if (memRequirements[0].size > 0)
        {
            data = device.mapMemory(stagingBufferMemory, this->_vertexBufferOffset, memRequirements[0].size);
            memcpy(data, this->_vertices.data(), static_cast<size_t>(vertexBufferSize));
            device.unmapMemory(stagingBufferMemory);
        }
        if (memRequirements[1].size > 0)
        {
            data = device.mapMemory(stagingBufferMemory, this->_indexBufferOffset, memRequirements[1].size);
            memcpy(data, this->_indices.data(), static_cast<size_t>(indexBufferSize));
            device.unmapMemory(stagingBufferMemory);
        }
        if (memRequirements[2].size > 0)
        {
            data = device.mapMemory(stagingBufferMemory, this->_uniformBufferOffset, memRequirements[2].size);
            memcpy(data, &this->_material->getUniformBuffer(), static_cast<size_t>(uniformBufferSize));
            device.unmapMemory(stagingBufferMemory);
        }
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), this->_uniformBufferOffset + uniformBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eUniformBuffer);
        this->_buffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memRequirements[0] = device.getBufferMemoryRequirements(this->_buffer);
        allocInfo = vk::MemoryAllocateInfo(memRequirements[0].size, Tools::getMemoryType(memProperties, memRequirements[0].memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
        if (this->_buffersMemory != (vk::DeviceMemory)VK_NULL_HANDLE)
            device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
        this->_buffersMemory = device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
        device.bindBufferMemory(this->_buffer, this->_buffersMemory, 0);
        vk::CommandBuffer commandBuffer = Tools::beginSingleTimeCommands(device, commandPool);
        commandBuffer.copyBuffer(stagingBuffer, this->_buffer, vk::BufferCopy(0, 0, this->_uniformBufferOffset + uniformBufferSize));
        Tools::endSingleTimeCommands(device, graphicsQueue, commandPool, commandBuffer);

        device.freeMemory(stagingBufferMemory, CUSTOM_ALLOCATOR);
        device.destroyBuffer(stagingBuffer, CUSTOM_ALLOCATOR);
        device.destroyBuffer(uniformBuffer, CUSTOM_ALLOCATOR);
        device.destroyBuffer(indexBuffer, CUSTOM_ALLOCATOR);
        device.destroyBuffer(vertexBuffer, CUSTOM_ALLOCATOR);

        this->_material->buildDescriptorSet(this->_buffer, this->_uniformBufferOffset);

        vk::CommandBufferAllocateInfo cmbBufferAllocInfo(commandPool, vk::CommandBufferLevel::eSecondary, 1);
        this->_commandBuffer = device.allocateCommandBuffers(cmbBufferAllocInfo).at(0);
    }

    void Submesh::buildCommandBuffer(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp)
    {
        this->_commandBuffer.reset(vk::CommandBufferResetFlags());
        vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse, &inheritanceInfo);
        this->_commandBuffer.begin(beginInfo);
        this->_commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f));
        this->_commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(1280, 720)));
        this->_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->_material->getPipeline());

        this->_commandBuffer.pushConstants<glm::mat4>(this->_material->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, mvp);

        this->_commandBuffer.bindVertexBuffers(0, this->_buffer, this->_vertexBufferOffset);
        this->_commandBuffer.bindIndexBuffer(this->_buffer, this->_indexBufferOffset, vk::IndexType::eUint32);
        this->_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, this->_material->getPipelineLayout(), 0, this->_material->getDescriptorSet(), nullptr);
        this->_commandBuffer.drawIndexed(static_cast<uint32_t>(this->_indices.size()), 1, 0, 0, 0);
        this->_commandBuffer.end();
    }

    void Submesh::setVertices(const std::vector<Vertex> &vertices)
    {
        this->_vertices = vertices;
    }

    void Submesh::setIndices(const std::vector<uint32_t> &indices)
    {
        this->_indices = indices;
    }

    void Submesh::setCommandBuffer(vk::CommandBuffer commandBuffer)
    {
        this->_commandBuffer = commandBuffer;
    }

    vk::CommandBuffer Submesh::getCommandBuffer() const
    {
        return (this->_commandBuffer);
    }

}
