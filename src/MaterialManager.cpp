#include "MaterialManager.h"
#include "ModelManager.h"

namespace Dwarf
{
	MaterialManager::MaterialManager()
	{
	}

	MaterialManager::~MaterialManager()
	{
    }

    size_t MaterialManager::addMaterial(const aiMaterial *material)
    {
        this->_materials.push_back(std::make_unique<Material>(this->_materials.size()));
        aiString name;
        aiColor3D color;
        float value;
        bool diffuseTexture = (material->GetTextureCount(aiTextureType_DIFFUSE) > 0);
        material->Get(AI_MATKEY_NAME, name);
        material->Get(AI_MATKEY_COLOR_AMBIENT, color); // Ka
        this->_materials.back()->setAmbient(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color); // Kd
        this->_materials.back()->setDiffuse(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_SPECULAR, color); // Ks
        this->_materials.back()->setSpecular(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color); // Ke
        this->_materials.back()->setEmissive(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_TRANSPARENT, color); // Tf
        this->_materials.back()->setTransmissionFilter(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_SHININESS, value); // Ns
        this->_materials.back()->setSpecularExponent(value);
        material->Get(AI_MATKEY_REFRACTI, value); // Ni
        this->_materials.back()->setRefraction(value);
        material->Get(AI_MATKEY_OPACITY, value); // d ?
        this->_materials.back()->setDissolve(value);
        if (diffuseTexture)
        {
            aiString texturePath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
            {
                this->_materials.back()->setTextureID(this->_textureManager.addTexture(texturePath.C_Str()));
                LOG(INFO) << "[" << name.C_Str() << "] Texture path: " << texturePath.C_Str();
            }
            else
                LOG(ERROR) << "Error getting texture";
        }
        return (this->_materials.back()->getID());
    }

    void MaterialManager::createMaterials(AllocationManager &allocationManager, const std::vector<ModelData> &modelDatas, std::vector<ModelVulkanData> &modelVulkanDatas)
    {
        std::vector<AllocationManager::BufferAllocInfo> uniformBufferAllocInfos;
        size_t i = 0;

        for (const auto &modelData : modelDatas)
        {
            for (const auto &meshData : modelData.meshDatas)
            {
                uniformBufferAllocInfos.push_back(AllocationManager::BufferAllocInfo(this->_materials.at(meshData.materialID)->getData()));
            }
        }
        allocationManager.createBuffer(uniformBufferAllocInfos, vk::BufferUsageFlagBits::eUniformBuffer);
        for (auto &modelVulkanData : modelVulkanDatas)
        {
            for (auto &meshVulkanData : modelVulkanData.meshVulkanDatas)
            {
                meshVulkanData.uniformBufferID = uniformBufferAllocInfos.at(i).bufferID;
                meshVulkanData.uniformBufferOffset = uniformBufferAllocInfos.at(i).offset;
                ++i;
            }
        }
        this->_textureManager.createTextures(allocationManager);
    }
}
