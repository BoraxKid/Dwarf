#include "ModelManager.h"

namespace Dwarf
{
    ModelManager::ModelManager()
    {
        this->_materialManager = std::make_shared<MaterialManager>();
        this->_modelLoader = std::make_unique<ModelLoader>(this->_materialManager, this->_modelDatas);
        this->loadModels();
    }

    ModelManager::~ModelManager()
    {
    }

    void ModelManager::loadModels()
    {
        this->_modelLoader->loadModel("resources/models/sphere.obj");
        for (const auto &modelData : this->_modelDatas)
        {
            for (const auto &meshData : modelData.meshDatas)
            {

            }
        }
    }
}
