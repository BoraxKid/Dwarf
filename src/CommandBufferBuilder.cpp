#include "CommandBufferBuilder.h"

namespace Dwarf
{
    CommandBufferBuilder::CommandBufferBuilder(const vk::Device &device, const vk::RenderPass &renderPass, std::vector<vk::Framebuffer> &swapChainFramebuffers, const vk::Extent2D &swapChainExtent, ThreadPool &threadPool, const uint32_t &numThreads)
        : _device(device), _renderPass(renderPass), _swapChainFramebuffers(swapChainFramebuffers), _swapChainExtent(swapChainExtent), _threadPool(threadPool), _numThreads(numThreads)
    {
    }

    CommandBufferBuilder::~CommandBufferBuilder()
    {
        for (const auto &commandPool : this->_commandPools)
        {
            if (commandPool)
                this->_device.destroyCommandPool(commandPool, CUSTOM_ALLOCATOR);
        }
    }

    void CommandBufferBuilder::createCommandPools(const uint32_t &graphicsFamily)
    {
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamily);
        this->_commandPools.resize(this->_numThreads);
        for (auto &commandPool : this->_commandPools)
        {
            if (commandPool)
                this->_device.destroyCommandPool(commandPool, CUSTOM_ALLOCATOR);
            commandPool = this->_device.createCommandPool(poolInfo, CUSTOM_ALLOCATOR);
        }
    }

    void CommandBufferBuilder::createCommandBuffers(const vk::Queue &graphicsQueue, vk::PhysicalDeviceMemoryProperties &memProperties)
    {
        uint32_t minObjectsPerThread = static_cast<uint32_t>(this->_buildables.size()) / this->_numThreads;
        this->_orderedBuildables.clear();
        this->_orderedBuildables.resize(this->_numThreads);
        uint32_t i = 0;
        uint32_t j;

        for (auto &buildable : this->_buildables)
        {
            if (i >= this->_numThreads)
                i = 0;
            this->_orderedBuildables.at(i).push_back(buildable);
            ++i;
        }
        std::vector<vk::CommandBuffer> commandBuffers;
        vk::CommandBufferAllocateInfo cmdBufferAllocInfo;

        i = 0;
        for (auto &buildables : this->_orderedBuildables)
        {
            if (!buildables.empty())
            {
                cmdBufferAllocInfo = vk::CommandBufferAllocateInfo(this->_commandPools.at(i), vk::CommandBufferLevel::eSecondary, static_cast<uint32_t>(buildables.size()));
                commandBuffers = this->_device.allocateCommandBuffers(cmdBufferAllocInfo);
                j = 0;
                for (auto &buildable : buildables)
                {
                    buildable->setCommandPool(&this->_commandPools.at(i));
                    buildable->createBuffers(this->_device, graphicsQueue, memProperties);
                    buildable->setCommandBuffer(commandBuffers.at(j));
                    ++j;
                }
            }
            ++i;
        }
    }

    void CommandBufferBuilder::buildCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const glm::mat4 &mvp)
    {
        std::vector<vk::CommandBuffer> builtCommandBuffers;
        vk::CommandBufferBeginInfo beginInfo;
        std::array<vk::ClearValue, 2> clearValues = { vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})), vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)) };
        vk::RenderPassBeginInfo renderPassInfo(this->_renderPass, vk::Framebuffer(), vk::Rect2D(vk::Offset2D(0, 0), this->_swapChainExtent), static_cast<uint32_t>(clearValues.size()), clearValues.data());
        vk::CommandBufferInheritanceInfo inheritanceInfo(this->_renderPass);
        uint32_t i = 0;
        uint32_t j;
        for (const auto &commandBuffer : commandBuffers)
        {
            builtCommandBuffers.clear();
            renderPassInfo.framebuffer = this->_swapChainFramebuffers.at(i);
            inheritanceInfo.framebuffer = this->_swapChainFramebuffers.at(i);
            commandBuffer.begin(beginInfo);
            commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
            j = 0;
            for (const auto &buildables : this->_orderedBuildables)
            {
                for (const auto &buildable : buildables)
                    this->_threadPool.addJobThread(j, std::bind(&IBuildable::buildCommandBuffer, buildable, inheritanceInfo, mvp, this->_swapChainExtent));
                ++j;
            }
            this->_threadPool.wait();
            for (const auto &buildable : this->_buildables)
                builtCommandBuffers.push_back(buildable->getCommandBuffer());
            commandBuffer.executeCommands(builtCommandBuffers);
            commandBuffer.endRenderPass();
            commandBuffer.end();
            ++i;
        }
    }

    void CommandBufferBuilder::addBuildable(IBuildable *buildable)
    {
        this->_buildables.push_back(buildable);
    }

    void CommandBufferBuilder::addBuildables(std::vector<IBuildable*> &buildables)
    {
        this->_buildables.insert(this->_buildables.end(), buildables.begin(), buildables.end());
    }
}
