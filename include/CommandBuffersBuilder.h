#ifndef DWARF_COMMANDBUFFERSBUILDER_H_
#define DWARF_COMMANDBUFFERSBUILDER_H_
#pragma once

#include <vector>

#include "Tools.h"
#include "IBuildable.h"
#include "ThreadPool.h"

namespace Dwarf
{
    class CommandBuffersBuilder
    {
    public:
        CommandBuffersBuilder(const vk::Device &device, const vk::RenderPass &renderPass, std::vector<vk::Framebuffer> &swapChainFramebuffers, const vk::Extent2D &swapChainExtent, ThreadPool &threadPool, const uint32_t &numThreads);
        virtual ~CommandBuffersBuilder();
        void createCommandPools(const uint32_t &graphicsFamily);
        void createCommandBuffers(const vk::Queue &graphicsQueue, vk::PhysicalDeviceMemoryProperties &memProperties);
        void buildCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const glm::mat4 &mvp);
        void addBuildable(IBuildable *buildable);
        void addBuildables(std::vector<IBuildable *> &buildables);

    private:
        const vk::Device &_device;
        const vk::RenderPass &_renderPass;
        std::vector<vk::Framebuffer> &_swapChainFramebuffers;
        const vk::Extent2D &_swapChainExtent;
        ThreadPool &_threadPool;
        const uint32_t &_numThreads;
        std::vector<IBuildable *> _buildables;
        std::vector<std::vector<IBuildable *>> _orderedBuildables;
        std::vector<vk::CommandPool> _commandPools;
    };
}

#endif // DWARF_COMMANDBUFFERSBUILDER_H_
