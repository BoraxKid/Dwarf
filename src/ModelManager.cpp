#include "ModelManager.h"

namespace Dwarf
{
    ModelManager::ModelManager()
    {
        this->_materialManager = std::make_shared<MaterialManager>();
        this->_modelLoader = std::make_unique<ModelLoader>(this->_materialManager, this->_modelDatas);
    }

    ModelManager::~ModelManager()
    {
    }

    void ModelManager::loadModels(AllocationManager &allocationManager)
    {
        std::vector<AllocationManager::BufferAllocInfo> vertexBufferAllocInfos;
        std::vector<AllocationManager::BufferAllocInfo> indexBufferAllocInfos;
        size_t i = 0;
        
        this->_modelLoader->loadModel("resources/models/sphere.obj");
        for (const auto &modelData : this->_modelDatas)
        {
            this->_modelVulkanDatas.push_back(ModelVulkanData());
            for (const auto &meshData : modelData.meshDatas)
            {
                this->_modelVulkanDatas.back().meshVulkanDatas.push_back(MeshVulkanData());
                vertexBufferAllocInfos.push_back(AllocationManager::BufferAllocInfo(meshData.vertices));
                indexBufferAllocInfos.push_back(AllocationManager::BufferAllocInfo(meshData.indices));
            }
        }
        allocationManager.createBuffer(vertexBufferAllocInfos, vk::BufferUsageFlagBits::eVertexBuffer);
        allocationManager.createBuffer(indexBufferAllocInfos, vk::BufferUsageFlagBits::eIndexBuffer);
        for (auto &modelVulkanData : this->_modelVulkanDatas)
        {
            for (auto &meshVulkanData : modelVulkanData.meshVulkanDatas)
            {
                meshVulkanData.vertexBufferOffset = vertexBufferAllocInfos.at(i).offset;
                meshVulkanData.indexBufferOffset = indexBufferAllocInfos.at(i).offset;
                ++i;
            }
        }
    }
}
