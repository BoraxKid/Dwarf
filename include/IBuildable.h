#ifndef IBUILDABLE_H_
#define IBUILDABLE_H_
#pragma once

#include <vulkan\vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Dwarf
{
    class IBuildable
    {
    public:
        virtual ~IBuildable() {}
        virtual void createBuffers(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::PhysicalDeviceMemoryProperties &memProperties) = 0;
        virtual void buildCommandBuffer(const vk::CommandBufferInheritanceInfo &inheritanceInfo, const glm::mat4 &mvp, const vk::Extent2D &extent) const = 0;
        virtual void setCommandPool(vk::CommandPool *commandPool) = 0;
        virtual void setCommandBuffer(vk::CommandBuffer commandBuffer) = 0;
        virtual vk::CommandBuffer getCommandBuffer() const = 0;
    };
}

#endif // IBUILDABLE_H_
