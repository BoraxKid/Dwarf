#ifndef DWARF_LIGHTMANAGER_H_
#define DWARF_LIGHTMANAGER_H_
#pragma once

#include "Tools.h"
#include "Light.h"

namespace Dwarf
{
    class LightManager
    {
    public:
        LightManager(const vk::Device &device, const vk::PhysicalDeviceMemoryProperties &memProperties);
        virtual ~LightManager();

        void buildDescriptorSet(const vk::PhysicalDeviceMemoryProperties &memProperties);
        void updateLightPos(float elapsedTime);
        const vk::DescriptorBufferInfo &getDescriptorBufferInfo() const;

    private:
        const vk::Device &_device;
        vk::DescriptorBufferInfo _descriptorBufferInfo;
        float _speed;
        float _angle;
        Light _light;
        vk::DeviceMemory _bufferMemory;
        vk::Buffer _uniformBuffer;
        void *_mappedData;
    };
}

#endif // DWARF_LIGHTMANAGER_H_
