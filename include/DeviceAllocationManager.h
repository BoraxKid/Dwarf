#ifndef DWARF_DEVICEALLOCATIONMANAGER_H_
#define DWARF_DEVICEALLOCATIONMANAGER_H_
#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "Mesh.h"

namespace Dwarf
{
    class DeviceAllocationManager
    {
    public:
        DeviceAllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties);
        virtual ~DeviceAllocationManager();
        void allocate(const std::vector<Mesh *> &meshes, const vk::CommandPool &commandPool);

    private:
        vk::MemoryRequirements getMemoryRequirements(const vk::DeviceSize &size, const vk::BufferUsageFlags &usage) const;

        const vk::Device &_device;
        const vk::Queue &_graphicsQueue;
        const vk::PhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties;
        std::vector<vk::DeviceMemory> _deviceMemories;
        std::vector<vk::Buffer> _buffers;
    };
}

#endif // DWARF_DEVICEALLOCATIONMANAGER_H_
