#include "SceneManager.h"

namespace Dwarf
{
    SceneManager::SceneManager(MaterialManager &materialManager)
        : _modelManager(materialManager)
    {
    }

    SceneManager::~SceneManager()
    {
    }
}
