#include "Submesh.h"

namespace Dwarf
{
    Submesh::Submesh(Material *material, const glm::mat4 &transformMatrix, const vk::DescriptorBufferInfo &lightBufferInfo)
        : _lightBufferInfo(lightBufferInfo), _material(material), _transform(transformMatrix)
    {
    }

    Submesh::~Submesh()
    {
    }

    void Submesh::cleanup(const vk::Device &device) const
    {
        device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
        device.destroyBuffer(this->_uniformBuffer, CUSTOM_ALLOCATOR);
    }

    void Submesh::createBuffers(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &memProperties)
    {
        vk::DeviceSize uniformBufferSize = sizeof(MaterialUniformBuffer);
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setSize(uniformBufferSize);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
        vk::Buffer uniformBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);

        vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(uniformBuffer);

        this->_uniformBufferOffset = 0;

        bufferInfo.setSize(memRequirements.size);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);

        vk::Buffer stagingBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memRequirements = device.getBufferMemoryRequirements(stagingBuffer);
        
        vk::MemoryAllocateInfo allocInfo(memRequirements.size, Tools::getMemoryType(memProperties, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        vk::DeviceMemory stagingBufferMemory = device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);

        device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
        void *data;
        if (memRequirements.size > 0)
        {
            data = device.mapMemory(stagingBufferMemory, this->_uniformBufferOffset, uniformBufferSize);
            memcpy(data, &this->_material->getUniformBuffer(), static_cast<size_t>(uniformBufferSize));
            device.unmapMemory(stagingBufferMemory);
        }
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), memRequirements.size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer);
        if (this->_uniformBuffer)
            device.destroyBuffer(this->_uniformBuffer, CUSTOM_ALLOCATOR);
        this->_uniformBuffer = device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memRequirements = device.getBufferMemoryRequirements(this->_uniformBuffer);
        allocInfo = vk::MemoryAllocateInfo(memRequirements.size, Tools::getMemoryType(memProperties, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
        if (this->_buffersMemory)
            device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
        this->_buffersMemory = device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
        device.bindBufferMemory(this->_uniformBuffer, this->_buffersMemory, 0);
        vk::CommandBuffer commandBuffer = Tools::beginSingleTimeCommands(device, *this->_commandPool);
        commandBuffer.copyBuffer(stagingBuffer, this->_uniformBuffer, vk::BufferCopy(0, 0, uniformBufferSize));
        Tools::endSingleTimeCommands(device, graphicsQueue, *this->_commandPool, commandBuffer);

        device.freeMemory(stagingBufferMemory, CUSTOM_ALLOCATOR);
        device.destroyBuffer(stagingBuffer, CUSTOM_ALLOCATOR);
        device.destroyBuffer(uniformBuffer, CUSTOM_ALLOCATOR);

        this->_material->buildDescriptorSet(this->_uniformBuffer, this->_uniformBufferOffset, memProperties, this->_lightBufferInfo);
    }

    void Submesh::buildCommandBuffer(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp, const vk::Extent2D &extent) const
    {
        this->_commandBuffer.reset(vk::CommandBufferResetFlags());
        vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse, &inheritanceInfo);
        this->_commandBuffer.begin(beginInfo);
        this->_commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f));
        this->_commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent));
        this->_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->_material->getPipeline());

        std::array<glm::mat4, 2> tmp = { mvp, this->_transform };
        this->_commandBuffer.pushConstants<glm::mat4>(this->_material->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, tmp);

        this->_commandBuffer.bindVertexBuffers(0, this->_buffer, this->_vertexBufferOffset);
        this->_commandBuffer.bindIndexBuffer(this->_buffer, this->_indexBufferOffset, vk::IndexType::eUint32);
        this->_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, this->_material->getPipelineLayout(), 0, this->_material->getDescriptorSet(), nullptr);
        this->_commandBuffer.drawIndexed(static_cast<uint32_t>(this->_indices.size()), 1, 0, 0, 0);
        this->_commandBuffer.end();
    }

    void Submesh::setCommandPool(vk::CommandPool *commandPool)
    {
        this->_commandPool = commandPool;
        this->_material->setCommandPool(commandPool);
    }

    void Submesh::setCommandBuffer(vk::CommandBuffer commandBuffer)
    {
        this->_commandBuffer = commandBuffer;
    }

    void Submesh::setVertices(const std::vector<Vertex> &vertices)
    {
        this->_vertices = vertices;
    }

    void Submesh::setIndices(const std::vector<uint32_t> &indices)
    {
        this->_indices = indices;
    }

    size_t Submesh::getVerticesCount() const
    {
        return (this->_vertices.size());
    }

    const std::vector<Vertex> &Submesh::getVertices() const
    {
        return (this->_vertices);
    }

    size_t Submesh::getIndicesCount() const
    {
        return (this->_indices.size());
    }

    const std::vector<uint32_t> &Submesh::getIndices() const
    {
        return (this->_indices);
    }

    void Submesh::setBuffer(const vk::Buffer &buffer)
    {
        this->_buffer = buffer;
    }

    void Submesh::setVertexBufferOffset(const vk::DeviceSize &vertexBufferOffset)
    {
        this->_vertexBufferOffset = vertexBufferOffset;
    }

    void Submesh::setIndexBufferOffset(const vk::DeviceSize &indexBufferOffset)
    {
        this->_indexBufferOffset = indexBufferOffset;
    }

    vk::CommandBuffer Submesh::getCommandBuffer() const
    {
        return (this->_commandBuffer);
    }

}
