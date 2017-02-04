#ifndef DWARF_MODELMANAGER_H_
#define DWARF_MODELMANAGER_H_
#pragma once

#include "ModelLoader.h"
#include "MaterialManager.h"
#include "ModelData.h"
#include "Model.h"

namespace Dwarf
{
    class ModelManager
    {
    public:
        ModelManager(MaterialManager &materialManager);
        virtual ~ModelManager();

    private:
        ModelLoader _modelLoader;
        std::unique_ptr<MaterialManager> _materialManager;
        std::vector<ModelData> _modelDatas;
        std::vector<Model> _models;
    };
}

#endif // DWARF_MODELMANAGER_H_
