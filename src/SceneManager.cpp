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
}
