#include "AllocationManager.h"

namespace Dwarf
{
    AllocationManager::AllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties)
        : _device(device), _graphicsQueue(graphicsQueue), _physicalDeviceMemoryProperties(physicalDeviceMemoryProperties)
    {
    }

    AllocationManager::~AllocationManager()
    {
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
}
