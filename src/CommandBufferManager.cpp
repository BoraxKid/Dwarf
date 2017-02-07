#include "CommandBufferManager.h"

namespace Dwarf
{
    CommandBufferManager::CommandBufferManager(AllocationManager &allocationManager)
        : _allocationManager(allocationManager)
    {
        this->_threadPool.setThreadCount(std::thread::hardware_concurrency());
    }

    CommandBufferManager::~CommandBufferManager()
    {
    }

    void CommandBufferManager::createCommandPools(uint32_t graphicsFamily)
    {
        this->_allocationManager.createCommandPools(graphicsFamily, this->_threadPool.getThreadCount());
    }

    void CommandBufferManager::buildCommandBuffers()
    {
    }
}
