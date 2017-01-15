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
        LightManager(const vk::Device &device);
        virtual ~LightManager();
        void setPipelineLayout(vk::PipelineLayout pipelineLayout);
        vk::DescriptorSetLayout createDescriptorSetLayout();
        void createDescriptorSet();
        void buildDescriptorSet(const vk::PhysicalDeviceMemoryProperties &memProperties);
        void updateLightPos(float elapsedTime);
        const vk::DescriptorSet &getDescriptorSet() const;

    private:
        const vk::Device &_device;
        vk::PipelineLayout _pipelineLayout;
        vk::DescriptorSetLayout _descriptorSetLayout;
        vk::DescriptorPool _descriptorPool;
        vk::DescriptorSet _descriptorSet;
        Light _light;
        vk::DeviceMemory _bufferMemory;
        vk::Buffer _uniformBuffer;
        void *_mappedData;
    };
}

#endif // DWARF_LIGHTMANAGER_H_
