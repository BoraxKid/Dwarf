#include "AllocationManager.h"

namespace Dwarf
{
    AllocationManager::AllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties)
        : _device(device), _graphicsQueue(graphicsQueue), _physicalDeviceMemoryProperties(physicalDeviceMemoryProperties)
    {
    }

    AllocationManager::~AllocationManager()
    {
        for (const auto &commandPool : this->_commandPools)
            this->_device.destroyCommandPool(commandPool, CUSTOM_ALLOCATOR);
        for (const auto &deviceMemory : this->_deviceMemories)
            this->_device.freeMemory(deviceMemory, CUSTOM_ALLOCATOR);
        for (const auto &buffer : this->_buffers)
            this->_device.destroyBuffer(buffer, CUSTOM_ALLOCATOR);
    }

    void AllocationManager::createCommandPools(uint32_t graphicsFamily, size_t size)
    {
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamily);
        for (const auto &commandPool : this->_commandPools)
        {
            if (commandPool)
                this->_device.destroyCommandPool(commandPool, CUSTOM_ALLOCATOR);
        }
        this->_commandPools.resize(size);
        for (auto &commandPool : this->_commandPools)
            commandPool = this->_device.createCommandPool(poolInfo, CUSTOM_ALLOCATOR);
    }

    void AllocationManager::createBuffer(std::vector<BufferAllocInfo> &bufferAllocInfos, const vk::BufferUsageFlags &usage)
    {
        vk::MemoryRequirements memoryRequirements = this->getMemoryRequirements(0, usage);
        vk::DeviceSize bufferSize = 0;
        vk::DeviceSize tmp = 0;
        vk::BufferCreateInfo bufferInfo;
        vk::Buffer stagingBuffer;
        vk::MemoryAllocateInfo memoryInfo;
        vk::DeviceMemory stagingMemory;
        void *data;
        vk::CommandBuffer singleUseCommandBuffer;

        for (auto &bufferAllocInfo : bufferAllocInfos)
        {
            tmp = bufferAllocInfo.size + (memoryRequirements.alignment - bufferAllocInfo.size % memoryRequirements.alignment);
            bufferAllocInfo.bufferSize = tmp;
            bufferSize += bufferAllocInfo.bufferSize;
        }
        memoryRequirements = this->getMemoryRequirements(bufferSize, usage);
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), memoryRequirements.size, vk::BufferUsageFlagBits::eTransferSrc);
        stagingBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        memoryRequirements = this->_device.getBufferMemoryRequirements(stagingBuffer);
        memoryInfo = vk::MemoryAllocateInfo(memoryRequirements.size, Tools::getMemoryType(this->_physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        stagingMemory = this->_device.allocateMemory(memoryInfo, CUSTOM_ALLOCATOR);
        this->_device.bindBufferMemory(stagingBuffer, stagingMemory, 0);
        tmp = 0;
        for (auto &bufferAllocInfo : bufferAllocInfos)
        {
            data = this->_device.mapMemory(stagingMemory, tmp, bufferAllocInfo.bufferSize);
            memcpy(data, bufferAllocInfo.data, bufferAllocInfo.size);
            this->_device.unmapMemory(stagingMemory);
            bufferAllocInfo.offset = tmp;
            tmp += bufferAllocInfo.bufferSize;
        }
        bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), memoryRequirements.size, vk::BufferUsageFlagBits::eTransferDst | usage);
        this->_buffers.push_back(this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR));
        for (auto &bufferAllocInfo : bufferAllocInfos)
            bufferAllocInfo.bufferID = this->_buffers.size() - 1;
        memoryRequirements = this->_device.getBufferMemoryRequirements(this->_buffers.back());
        memoryInfo = vk::MemoryAllocateInfo(memoryRequirements.size, Tools::getMemoryType(this->_physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
        this->_deviceMemories.push_back(this->_device.allocateMemory(memoryInfo, CUSTOM_ALLOCATOR));
        this->_device.bindBufferMemory(this->_buffers.back(), this->_deviceMemories.back(), 0);
        singleUseCommandBuffer = Tools::beginSingleTimeCommands(this->_device, this->_commandPools.at(0));
        singleUseCommandBuffer.copyBuffer(stagingBuffer, this->_buffers.back(), vk::BufferCopy(0, 0, memoryRequirements.size));
        Tools::endSingleTimeCommands(this->_device, this->_graphicsQueue, this->_commandPools.at(0), singleUseCommandBuffer);

        this->_device.freeMemory(stagingMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(stagingBuffer, CUSTOM_ALLOCATOR);
    }

    void AllocationManager::createImage(const void *imageData, vk::DeviceSize size, uint32_t width, uint32_t height)
    {
        vk::Image stagingImage;
        vk::DeviceMemory stagingImageMemory;
        Tools::createImage(this->_device, this->_physicalDeviceMemoryProperties, width, height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingImage, stagingImageMemory);
        void *data = this->_device.mapMemory(stagingImageMemory, 0, size);
        memcpy(data, imageData, static_cast<size_t>(size));
        this->_device.unmapMemory(stagingImageMemory);
        
        this->_images.push_back(vk::Image());
        Tools::createImage(this->_device, this->_physicalDeviceMemoryProperties, width, height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, this->_textureImage, this->_textureImageMemory);
        Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPools.at(0), stagingImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal);
        Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPools.at(0), this->_images.back(), vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);
        Tools::copyImage(this->_device, this->_graphicsQueue, this->_commandPools.at(0), stagingImage, this->_images.back(), width, height);
        Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPools.at(0), this->_images.back(), vk::ImageLayout::eTransferDstOptimal, this->_textureImageLayout);
        this->_device.freeMemory(stagingImageMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyImage(stagingImage, CUSTOM_ALLOCATOR);
        Tools::createImageView(this->_device, this->_images.back(), vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, this->_textureImageView);
        vk::SamplerCreateInfo samplerInfo(vk::SamplerCreateFlags(), vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f, VK_TRUE, 16, VK_FALSE, vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
        this->_textureSampler = this->_device.createSampler(samplerInfo, CUSTOM_ALLOCATOR);

        this->_imageInfo = vk::DescriptorImageInfo(this->_textureSampler, this->_textureImageView, this->_textureImageLayout);
    }

    vk::MemoryRequirements AllocationManager::getMemoryRequirements(const vk::DeviceSize &size, const vk::BufferUsageFlags &usage) const
    {
        vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), size, usage);
        vk::Buffer temporaryBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        vk::MemoryRequirements memoryRequirements = this->_device.getBufferMemoryRequirements(temporaryBuffer);
        this->_device.destroyBuffer(temporaryBuffer, CUSTOM_ALLOCATOR);
        return (memoryRequirements);
    }
}
