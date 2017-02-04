#include "ModelManager.h"

namespace Dwarf
{
    ModelManager::ModelManager(MaterialManager &materialManager)
        : _modelLoader(materialManager)
    {
    }

    ModelManager::~ModelManager()
    {
    }
}
