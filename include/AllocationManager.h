#ifndef DWARF_ALLOCATIONMANAGER_H_
#define DWARF_ALLOCATIONMANAGER_H_
#pragma once

#include "Tools.h"
#include "Model.h"

namespace Dwarf
{
    class AllocationManager
    {
    public:
        AllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties);
        virtual ~AllocationManager();

        void createCommandPools(uint32_t graphicsFamily, size_t size);

    private:
        const vk::Device &_device;
        const vk::Queue &_graphicsQueue;
        const vk::PhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties;
        std::vector<vk::CommandPool> _commandPools;
        std::vector<vk::DeviceMemory> _deviceMemories;
        std::vector<vk::Buffer> _buffers;
    };
}

#endif // DWARF_ALLOCATIONMANAGER_H_
