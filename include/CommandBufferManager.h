#ifndef DWARF_COMMANDBUFFERMANAGER_H_
#define DWARF_COMMANDBUFFERMANAGER_H_
#pragma once

#include "AllocationManager.h"
#include "ThreadPool.h"

namespace Dwarf
{
    class CommandBufferManager
    {
    public:
        CommandBufferManager(AllocationManager &allocationManager);
        virtual ~CommandBufferManager();

        void createCommandPools(uint32_t graphicsFamily);
        void buildCommandBuffers();

    private:
        AllocationManager &_allocationManager;
        ThreadPool _threadPool;
    };
}

#endif // DWARF_COMMANDBUFFERMANAGER_H_
