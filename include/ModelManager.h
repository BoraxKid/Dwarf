#ifndef DWARF_MODELMANAGER_H_
#define DWARF_MODELMANAGER_H_
#pragma once

#include "ModelLoader.h"
#include "AllocationManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "ModelData.h"
#include "Model.h"

namespace Dwarf
{
    struct MeshVulkanData
    {
        size_t commandBufferID;
        size_t vertexBufferID;
        vk::DeviceSize vertexBufferOffset;
        size_t indexBufferID;
        vk::DeviceSize indexBufferOffset;
        size_t uniformBufferID;
        vk::DeviceSize uniformBufferOffset;
    };

    struct ModelVulkanData
    {
        std::vector<MeshVulkanData> meshVulkanDatas;
    };

    class ModelManager
    {
    public:
        ModelManager(const vk::Device &device);
        virtual ~ModelManager();

    private:
        void loadModels(AllocationManager &allocationManager);

        ModelLoader _modelLoader;
        MaterialManager _materialManager;
        std::vector<ModelData> _modelDatas;
        std::vector<ModelVulkanData> _modelVulkanDatas;
        std::vector<Model> _models;
    };
}

#endif // DWARF_MODELMANAGER_H_
