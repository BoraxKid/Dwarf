#ifndef DWARF_SCENEMANAGER_H_
#define DWARF_SCENEMANAGER_H_
#pragma once

#include "ModelManager.h"

namespace Dwarf
{
    class SceneManager
    {
    public:
        SceneManager(const vk::Device &device);
        virtual ~SceneManager();

        void init(AllocationManager &allocationManager, const vk::Extent2D &extent, const vk::RenderPass &renderPass);

    private:
        ModelManager _modelManager;
    };
}

#endif // DWARF_SCENEMANAGER_H_
