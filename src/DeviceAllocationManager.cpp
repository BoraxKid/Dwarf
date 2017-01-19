#include "DeviceAllocationManager.h"

namespace Dwarf
{
    DeviceAllocationManager::DeviceAllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties)
        : _device(device), _graphicsQueue(graphicsQueue), _physicalDeviceMemoryProperties(physicalDeviceMemoryProperties)
    {
    }

    DeviceAllocationManager::~DeviceAllocationManager()
    {
    }

    void DeviceAllocationManager::allocate(const std::vector<Mesh *> &meshes, const vk::CommandPool &commandPool)
    {
        vk::DeviceSize totalVertexBuffersSize = 0;
        std::vector<vk::DeviceSize> separateVertexBuffersSizes;
        vk::DeviceSize totalIndexBuffersSize = 0;
        std::vector<vk::DeviceSize> separateIndexBuffersSizes;
        vk::DeviceSize tmp;

        for (const auto &mesh : meshes)
        {
            for (const auto &submesh : mesh->getSubmeshes())
            {
                tmp = sizeof(Vertex) * submesh.getVerticesCount();
                if (tmp > 0)
                {
                    totalVertexBuffersSize += tmp;
                    separateVertexBuffersSizes.push_back(tmp);
                    tmp = sizeof(uint32_t) * submesh.getIndicesCount();
                    totalIndexBuffersSize += tmp;
                    separateIndexBuffersSizes.push_back(tmp);
                }
            }
        }
        vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), totalVertexBuffersSize, vk::BufferUsageFlagBits::eVertexBuffer);
        vk::Buffer temporaryVertexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        vk::MemoryRequirements memoryRequirements = this->_device.getBufferMemoryRequirements(temporaryVertexBuffer);
        this->_device.destroyBuffer(temporaryVertexBuffer, CUSTOM_ALLOCATOR);

        totalVertexBuffersSize = 0;
        for (auto &vertexBufferSize : separateVertexBuffersSizes)
        {
            tmp = (vertexBufferSize + (memoryRequirements.alignment - vertexBufferSize % memoryRequirements.alignment));
            if (vertexBufferSize != tmp)
                vertexBufferSize = tmp;
            totalVertexBuffersSize += vertexBufferSize;
        }
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), totalVertexBuffersSize, vk::BufferUsageFlagBits::eVertexBuffer);
        temporaryVertexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memoryRequirements = this->_device.getBufferMemoryRequirements(temporaryVertexBuffer);
        this->_device.destroyBuffer(temporaryVertexBuffer, CUSTOM_ALLOCATOR);
        LOG(INFO) << "Size needed: " << totalVertexBuffersSize << " and size form requirements: " << memoryRequirements.size;

        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), memoryRequirements.size, vk::BufferUsageFlagBits::eTransferSrc);
        vk::Buffer stagingBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memoryRequirements = this->_device.getBufferMemoryRequirements(stagingBuffer);
        LOG(INFO) << "Size from requirements: " << memoryRequirements.size;
        vk::MemoryAllocateInfo allocInfo(memoryRequirements.size, Tools::getMemoryType(this->_physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        vk::DeviceMemory stagingBufferMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);

        this->_device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
        void *data;
        tmp = 0;
        size_t i = 0;
        for (const auto &mesh : meshes)
        {
            for (const auto &submesh : mesh->getSubmeshes())
            {
                if (sizeof(Vertex) * submesh.getVerticesCount() > 0)
                {
                    data = this->_device.mapMemory(stagingBufferMemory, tmp, static_cast<vk::DeviceSize>(sizeof(Vertex) * submesh.getVerticesCount()));
                    memcpy(data, submesh.getVertices().data(), static_cast<size_t>(sizeof(Vertex) * submesh.getVerticesCount()));
                    this->_device.unmapMemory(stagingBufferMemory);
                    tmp += separateVertexBuffersSizes.at(i);
                    ++i;
                }
            }
        }
        
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), memoryRequirements.size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
        this->_buffers.push_back(this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR));
        memoryRequirements = this->_device.getBufferMemoryRequirements(this->_buffers.back());
        LOG(INFO) << "Size from requirements: " << memoryRequirements.size;
        allocInfo = vk::MemoryAllocateInfo(memoryRequirements.size, Tools::getMemoryType(this->_physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
        this->_deviceMemories.push_back(this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR));
        
        this->_device.bindBufferMemory(this->_buffers.back(), this->_deviceMemories.back(), 0);
        vk::CommandBuffer singleUseCommandBuffer = Tools::beginSingleTimeCommands(this->_device, commandPool);
        singleUseCommandBuffer.copyBuffer(stagingBuffer, this->_buffers.back(), vk::BufferCopy(0, 0, memoryRequirements.size));
        Tools::endSingleTimeCommands(this->_device, this->_graphicsQueue, commandPool, singleUseCommandBuffer);

        this->_device.freeMemory(this->_deviceMemories.back(), CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(this->_buffers.back(), CUSTOM_ALLOCATOR);
        this->_device.freeMemory(stagingBufferMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(stagingBuffer);
    }
}
