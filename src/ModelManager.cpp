#include "ModelManager.h"

namespace Dwarf
{
    ModelManager::ModelManager(const vk::Device &device)
        : _materialManager(device), _modelLoader(_materialManager, _modelDatas)
    {
    }

    ModelManager::~ModelManager()
    {
    }

    void ModelManager::init()
    {
        this->_materialManager.init();
    }

    void ModelManager::loadModels(AllocationManager &allocationManager, const vk::Extent2D &extent, const vk::RenderPass &renderPass)
    {
        std::vector<AllocationManager::BufferAllocInfo> vertexBufferAllocInfos;
        std::vector<AllocationManager::BufferAllocInfo> indexBufferAllocInfos;
        size_t i = 0;
        
        this->_modelLoader.loadModel("resources/models/sphere.obj");
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
                meshVulkanData.vertexBufferID = vertexBufferAllocInfos.at(i).bufferID;
                meshVulkanData.vertexBufferOffset = vertexBufferAllocInfos.at(i).offset;
                meshVulkanData.indexBufferID = indexBufferAllocInfos.at(i).bufferID;
                meshVulkanData.indexBufferOffset = indexBufferAllocInfos.at(i).offset;
                ++i;
            }
        }
        this->_materialManager.createMaterials(allocationManager, this->_modelDatas, this->_modelVulkanDatas);
        this->_materialManager.recreatePipelines(extent, renderPass);
    }
}
