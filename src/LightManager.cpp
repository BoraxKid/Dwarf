#include "LightManager.h"

namespace Dwarf
{
    LightManager::LightManager(const vk::Device &device, const vk::PhysicalDeviceMemoryProperties &memProperties)
        : _device(device), _mappedData(nullptr), _light(0.0, 0.0, 0.0, Color(1.0f, 1.0f, 1.0f), 5.0), _speed(2.0f * 3.14f / 2.0f), _angle(0.0f)
    {
        this->buildDescriptorSet(memProperties);
    }

    LightManager::~LightManager()
    {
        this->_device.unmapMemory(this->_bufferMemory);
        this->_device.freeMemory(this->_bufferMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(this->_uniformBuffer, CUSTOM_ALLOCATOR);
    }

    void LightManager::buildDescriptorSet(const vk::PhysicalDeviceMemoryProperties &memProperties)
    {
        vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), sizeof(LightUniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer);
        this->_uniformBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
        vk::MemoryRequirements memRequirements = this->_device.getBufferMemoryRequirements(this->_uniformBuffer);
        vk::MemoryAllocateInfo memAllocInfo(memRequirements.size, Tools::getMemoryType(memProperties, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        this->_bufferMemory = this->_device.allocateMemory(memAllocInfo, CUSTOM_ALLOCATOR);
        this->_mappedData = this->_device.mapMemory(this->_bufferMemory, 0, sizeof(LightUniformBuffer));
        memcpy(this->_mappedData, &this->_light.getUniformBuffer(), sizeof(LightUniformBuffer));
        this->_device.bindBufferMemory(this->_uniformBuffer, this->_bufferMemory, 0);
        this->_descriptorBufferInfo = vk::DescriptorBufferInfo(this->_uniformBuffer, 0, sizeof(LightUniformBuffer));
    }

    void LightManager::updateLightPos(float elapsedTime)
    {
        this->_angle += this->_speed * elapsedTime;
        glm::dvec3 tmp(cos(this->_angle) * 8.0, 0.0, sin(this->_angle) * 8.0);
        this->_light.setPosition(tmp);
        memcpy(this->_mappedData, &this->_light.getUniformBuffer(), sizeof(LightUniformBuffer));
    }

    const vk::DescriptorBufferInfo &LightManager::getDescriptorBufferInfo() const
    {
        return (this->_descriptorBufferInfo);
    }
}
