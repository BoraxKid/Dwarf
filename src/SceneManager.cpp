#include "SceneManager.h"

namespace Dwarf
{
    SceneManager::SceneManager(const vk::Device &device)
        : _modelManager(device)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    void SceneManager::init(AllocationManager &allocationManager, const vk::Extent2D &extent, const vk::RenderPass &renderPass)
    {
        this->_modelManager.init();
        this->_modelManager.loadModels(allocationManager, extent, renderPass);
    }
}
