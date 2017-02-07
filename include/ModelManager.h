#ifndef DWARF_MODELMANAGER_H_
#define DWARF_MODELMANAGER_H_
#pragma once

#include "ModelLoader.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "ModelData.h"
#include "Model.h"

namespace Dwarf
{
    struct MeshVulkanData
    {
        size_t commandBufferID;
        vk::DeviceSize vertexBufferOffset;
        vk::DeviceSize indexBufferOffset;
        vk::DeviceSize uniformBufferOffset;
    };

    struct ModelVulkanData
    {
        std::vector<MeshVulkanData> _meshVulkanDatas;
    };

    class ModelManager
    {
    public:
        ModelManager();
        virtual ~ModelManager();

    private:
        void loadModels();

        std::unique_ptr<ModelLoader> _modelLoader;
        std::shared_ptr<MaterialManager> _materialManager;
        std::vector<ModelData> _modelDatas;
        std::vector<ModelVulkanData> _modelVulkanDatas;
        std::vector<Model> _models;
    };
}

#endif // DWARF_MODELMANAGER_H_
