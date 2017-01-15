#include "LightManager.h"

namespace Dwarf
{
    LightManager::LightManager(const vk::Device &device)
        : _device(device), _mappedData(nullptr), _light(0.0, 0.0, 0.0, Color(1.0f, 1.0f, 1.0f))
    {
    }

    LightManager::~LightManager()
    {
        this->_device.freeMemory(this->_bufferMemory, CUSTOM_ALLOCATOR);
        this->_device.destroyBuffer(this->_uniformBuffer, CUSTOM_ALLOCATOR);
        this->_device.freeDescriptorSets(this->_descriptorPool, this->_descriptorSet);
        this->_device.destroyDescriptorPool(this->_descriptorPool, CUSTOM_ALLOCATOR);
        this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
    }

    void LightManager::setPipelineLayout(vk::PipelineLayout pipelineLayout)
    {
        this->_pipelineLayout = pipelineLayout;
        this->createDescriptorSet();
    }

    vk::DescriptorSetLayout LightManager::createDescriptorSetLayout()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings =
        {
            vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
        };
        vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), static_cast<uint32_t>(bindings.size()), bindings.data());
        if (this->_descriptorSetLayout)
            this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
        this->_descriptorSetLayout = this->_device.createDescriptorSetLayout(layoutInfo, CUSTOM_ALLOCATOR);
        return (this->_descriptorSetLayout);
    }

    void LightManager::createDescriptorSet()
    {
        std::vector<vk::DescriptorPoolSize> poolSizes = { vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1) };
        vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlags(), 1, static_cast<uint32_t>(poolSizes.size()), poolSizes.data());
        this->_descriptorPool = this->_device.createDescriptorPool(poolInfo, CUSTOM_ALLOCATOR);
        vk::DescriptorSetAllocateInfo allocInfo(this->_descriptorPool, 1, &this->_descriptorSetLayout);
        std::vector<vk::DescriptorSet> descriptorSets = this->_device.allocateDescriptorSets(allocInfo);
        this->_descriptorSet = descriptorSets.at(0);
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
        vk::DescriptorBufferInfo descriptorBufferInfo(this->_uniformBuffer, 0, sizeof(LightUniformBuffer));
        std::vector<vk::WriteDescriptorSet> writeDescriptorSets = { vk::WriteDescriptorSet(this->_descriptorSet, 2, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &descriptorBufferInfo) };
        this->_device.updateDescriptorSets(writeDescriptorSets, nullptr);
        /*vk::DescriptorBufferInfo bufferInfo(buffer, uniformBufferOffset, sizeof(LightUniformBuffer));
        std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(this->_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo) };
        this->_device.updateDescriptorSets(descriptorWrites, nullptr);*/
    }

    void LightManager::updateLightPos(float elapsedTime)
    {
        glm::dvec3 tmp(7.5 * 1.1 * sin(glm::radians(static_cast<double>(elapsedTime) * 360)), -4.0f, 7.5f * 1.1 * cos(glm::radians(static_cast<double>(elapsedTime) * 360)));
        this->_light.setPosition(tmp);
        memcpy(this->_mappedData, &this->_light.getUniformBuffer(), sizeof(LightUniformBuffer));
    }

    const vk::DescriptorSet &LightManager::getDescriptorSet() const
    {
        return (this->_descriptorSet);
    }
}
