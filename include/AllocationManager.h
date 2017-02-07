#ifndef DWARF_ALLOCATIONMANAGER_H_
#define DWARF_ALLOCATIONMANAGER_H_
#pragma once

#include "Tools.h"
#include "Model.h"
#include "MeshData.h"

namespace Dwarf
{
    class AllocationManager
    {
    public:
        struct BufferAllocInfo
        {
            BufferAllocInfo(const std::vector<Vertex> &vertices)
                : data(vertices.data()), size(vertices.size() * sizeof(Vertex)), offset(0)
            {}

            BufferAllocInfo(const std::vector<uint32_t> &indices)
                : data(indices.data()), size(indices.size() * sizeof(uint32_t)), offset(0)
            {}

            const void *data;
            size_t size;
            vk::DeviceSize bufferSize;
            vk::DeviceSize offset;
        };

        AllocationManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties);
        virtual ~AllocationManager();

        void createCommandPools(uint32_t graphicsFamily, size_t size);
        void createBuffer(std::vector<BufferAllocInfo> &bufferAllocInfos, const vk::BufferUsageFlags & usage);

    private:
        vk::MemoryRequirements getMemoryRequirements(const vk::DeviceSize &size, const vk::BufferUsageFlags &usage) const;

        const vk::Device &_device;
        const vk::Queue &_graphicsQueue;
        const vk::PhysicalDeviceMemoryProperties &_physicalDeviceMemoryProperties;
        std::vector<vk::CommandPool> _commandPools;
        std::vector<vk::DeviceMemory> _deviceMemories;
        std::vector<vk::Buffer> _buffers;
    };
}

#endif // DWARF_ALLOCATIONMANAGER_H_
