#include "CommandBufferBuilder.h"

namespace Dwarf
{
    CommandBufferBuilder::CommandBufferBuilder(const vk::Device &device, const vk::RenderPass &renderPass, std::vector<vk::Framebuffer> &swapChainFramebuffers, const vk::Extent2D &swapChainExtent, ThreadPool &threadPool, const uint32_t &numThreads)
        : _device(device), _renderPass(renderPass), _swapChainFramebuffers(swapChainFramebuffers), _swapChainExtent(swapChainExtent), _threadPool(threadPool), _numThreads(numThreads)
    {
    }

    CommandBufferBuilder::~CommandBufferBuilder()
    {
        std::vector<vk::CommandPool>::iterator iter = this->_commandPools.begin();
        std::vector<vk::CommandPool>::iterator iterEnd = this->_commandPools.end();
        while (iter != iterEnd)
        {
            if ((*iter))
                this->_device.destroyCommandPool(*iter, CUSTOM_ALLOCATOR);
            ++iter;
        }
    }

    void CommandBufferBuilder::createCommandPools(const uint32_t &graphicsFamily)
    {
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamily);
        std::vector<vk::CommandPool>::iterator iter;
        std::vector<vk::CommandPool>::iterator iterEnd;
        this->_commandPools.resize(this->_numThreads);
        iter = this->_commandPools.begin();
        iterEnd = this->_commandPools.end();
        while (iter != iterEnd)
        {
            if ((*iter))
                this->_device.destroyCommandPool(*iter, CUSTOM_ALLOCATOR);
            (*iter) = this->_device.createCommandPool(poolInfo, CUSTOM_ALLOCATOR);
            ++iter;
        }
    }

    void CommandBufferBuilder::createCommandBuffers(const vk::Queue &graphicsQueue, vk::PhysicalDeviceMemoryProperties &memProperties)
    {
        uint32_t minObjectsPerThread = static_cast<uint32_t>(this->_buildables.size()) / this->_numThreads;
        this->_orderedBuildables.clear();
        this->_orderedBuildables.resize(this->_numThreads);
        std::vector<IBuildable *>::iterator iter = this->_buildables.begin();
        std::vector<IBuildable *>::iterator iterEnd = this->_buildables.end();
        uint32_t i = 0;
        while (iter != iterEnd)
        {
            if (i >= this->_numThreads)
                i = 0;
            this->_orderedBuildables.at(i).push_back(*iter);
            ++i;
            ++iter;
        }
        std::vector<vk::CommandBuffer> commandBuffers;
        vk::CommandBufferAllocateInfo cmdBufferAllocInfo;
        std::vector<std::vector<IBuildable *>>::iterator iter2 = this->_orderedBuildables.begin();
        std::vector<std::vector<IBuildable *>>::iterator iterEnd2 = this->_orderedBuildables.end();
        while (iter2 != iterEnd2)
        {
            cmdBufferAllocInfo = vk::CommandBufferAllocateInfo(this->_commandPools.at(iter2 - this->_orderedBuildables.begin()), vk::CommandBufferLevel::eSecondary, static_cast<uint32_t>(iter2->size()));
            commandBuffers = this->_device.allocateCommandBuffers(cmdBufferAllocInfo);
            i = 0;
            iter = iter2->begin();
            iterEnd = iter2->end();
            while (iter != iterEnd)
            {
                (*iter)->setCommandPool(&this->_commandPools.at(iter2 - this->_orderedBuildables.begin()));
                (*iter)->createBuffers(this->_device, graphicsQueue, memProperties);
                (*iter)->setCommandBuffer(commandBuffers.at(i));
                ++i;
                ++iter;
            }
            ++iter2;
        }
    }

    void CommandBufferBuilder::buildCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const glm::mat4 &mvp)
    {
        std::vector<vk::CommandBuffer> builtCommandBuffers;
        vk::CommandBufferBeginInfo beginInfo;
        std::array<vk::ClearValue, 2> clearValues = { vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})), vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)) };
        vk::RenderPassBeginInfo renderPassInfo(this->_renderPass, vk::Framebuffer(), vk::Rect2D(vk::Offset2D(0, 0), this->_swapChainExtent), static_cast<uint32_t>(clearValues.size()), clearValues.data());
        vk::CommandBufferInheritanceInfo inheritanceInfo(this->_renderPass);
        uint32_t objectsPerThread = static_cast<uint32_t>(this->_buildables.size()) / this->_numThreads;
        std::vector<std::vector<IBuildable *>>::iterator iterOrdered;
        std::vector<std::vector<IBuildable *>>::iterator iterOrderedEnd = this->_orderedBuildables.end();
        std::vector<IBuildable *>::iterator iterBuildables;
        std::vector<IBuildable *>::iterator iterBuildablesEnd;
        std::vector<vk::CommandBuffer>::const_iterator iter = commandBuffers.begin();
        std::vector<vk::CommandBuffer>::const_iterator iterEnd = commandBuffers.end();
        while (iter != iterEnd)
        {
            builtCommandBuffers.clear();
            iterOrdered = this->_orderedBuildables.begin();
            renderPassInfo.framebuffer = this->_swapChainFramebuffers.at(iter - commandBuffers.begin());
            inheritanceInfo.framebuffer = this->_swapChainFramebuffers.at(iter - commandBuffers.begin());
            iter->begin(beginInfo);
            iter->beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
            while (iterOrdered != iterOrderedEnd)
            {
                iterBuildables = iterOrdered->begin();
                iterBuildablesEnd = iterOrdered->end();
                while (iterBuildables != iterBuildablesEnd)
                {
                    this->_threadPool.addJobThread(static_cast<uint32_t>(iterOrdered - this->_orderedBuildables.begin()), std::bind(&IBuildable::buildCommandBuffer, *iterBuildables, inheritanceInfo, mvp));
                    //(*iterBuildables)->buildCommandBuffer(inheritanceInfo, mvp);
                    ++iterBuildables;
                }
                ++iterOrdered;
            }
            this->_threadPool.wait();
            iterBuildables = this->_buildables.begin();
            iterBuildablesEnd = this->_buildables.end();
            while (iterBuildables != iterBuildablesEnd)
            {
                builtCommandBuffers.push_back((*iterBuildables)->getCommandBuffer());
                ++iterBuildables;
            }
            iter->executeCommands(builtCommandBuffers);
            iter->endRenderPass();
            iter->end();
            ++iter;
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
