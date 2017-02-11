#include "MaterialManager.h"
#include "ModelManager.h"

namespace Dwarf
{
    MaterialManager::MaterialManager(const vk::Device &device)
        : _device(device), _materialLastID(0), _descriptorSetLastID(0), _pipelineLastID(0)
	{
	}

	MaterialManager::~MaterialManager()
	{
        for (const auto &pipeline : this->_pipelines)
            this->_device.destroyPipeline(pipeline.second, CUSTOM_ALLOCATOR);
        this->_pipelines.clear();
        if (!this->_descriptorSets.empty())
        {
            std::vector<vk::DescriptorSet> existingDescriptorSets;
            for (const auto &descriptorSet : this->_descriptorSets)
                existingDescriptorSets.push_back(descriptorSet.second);
            this->_device.freeDescriptorSets(this->_descriptorPool, existingDescriptorSets);
            this->_descriptorSets.clear();
        }
        this->_materials.clear();
        if (this->_descriptorPool)
            this->_device.destroyDescriptorPool(this->_descriptorPool, CUSTOM_ALLOCATOR);
        if (this->_pipelineLayout)
            this->_device.destroyPipelineLayout(this->_pipelineLayout, CUSTOM_ALLOCATOR);
        if (this->_descriptorSetLayout)
            this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
    }

    size_t MaterialManager::addMaterial(const aiMaterial *material)
    {
        this->_materials[++this->_materialLastID] = Material();
        aiString name;
        aiColor3D color;
        float value;
        bool diffuseTexture = (material->GetTextureCount(aiTextureType_DIFFUSE) > 0);
        material->Get(AI_MATKEY_NAME, name);
        material->Get(AI_MATKEY_COLOR_AMBIENT, color); // Ka
        this->_materials.at(this->_materialLastID).setAmbient(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color); // Kd
        this->_materials.at(this->_materialLastID).setDiffuse(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_SPECULAR, color); // Ks
        this->_materials.at(this->_materialLastID).setSpecular(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color); // Ke
        this->_materials.at(this->_materialLastID).setEmissive(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_COLOR_TRANSPARENT, color); // Tf
        this->_materials.at(this->_materialLastID).setTransmissionFilter(Color(color.r, color.g, color.b));
        material->Get(AI_MATKEY_SHININESS, value); // Ns
        this->_materials.at(this->_materialLastID).setSpecularExponent(value);
        material->Get(AI_MATKEY_REFRACTI, value); // Ni
        this->_materials.at(this->_materialLastID).setRefraction(value);
        material->Get(AI_MATKEY_OPACITY, value); // d ?
        this->_materials.at(this->_materialLastID).setDissolve(value);
        if (diffuseTexture)
        {
            aiString texturePath;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
            {
                this->_materials.at(this->_materialLastID).setTextureID(this->_textureManager.addTexture(texturePath.C_Str()));
                LOG(INFO) << "[" << name.C_Str() << "] Texture path: " << texturePath.C_Str();
            }
            else
                LOG(ERROR) << "Error getting texture";
        }
        return (this->_materialLastID);
    }

    void MaterialManager::createMaterials(AllocationManager &allocationManager, const std::vector<ModelData> &modelDatas, std::vector<ModelVulkanData> &modelVulkanDatas)
    {
        std::vector<AllocationManager::BufferAllocInfo> uniformBufferAllocInfos;
        size_t i = 0;

        for (const auto &modelData : modelDatas)
        {
            for (const auto &meshData : modelData.meshDatas)
            {
                uniformBufferAllocInfos.push_back(AllocationManager::BufferAllocInfo(this->_materials.at(meshData.materialID).getData()));
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

    void MaterialManager::createDescriptorPool()
    {
        uint32_t descriptorCount = static_cast<uint32_t>(this->_materials.size()) * 2;
        std::vector<vk::DescriptorPoolSize> poolSizes = { vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, descriptorCount), vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, descriptorCount) };
        vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlags(), descriptorCount, static_cast<uint32_t>(poolSizes.size()), poolSizes.data());
        this->_descriptorPool = this->_device.createDescriptorPool(poolInfo, CUSTOM_ALLOCATOR);
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts(this->_materials.size(), this->_descriptorSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo(this->_descriptorPool, static_cast<uint32_t>(this->_materials.size()), descriptorSetLayouts.data());
        std::vector<vk::DescriptorSet> descriptorSets = this->_device.allocateDescriptorSets(allocInfo);
        if (this->_descriptorSets.empty())
        {
            std::vector<vk::DescriptorSet> existingDescriptorSets;
            for (const auto &descriptorSet : this->_descriptorSets)
                existingDescriptorSets.push_back(descriptorSet.second);
            this->_device.freeDescriptorSets(this->_descriptorPool, existingDescriptorSets);
            this->_descriptorSets.clear();
        }
        size_t i = 0;
        for (auto &material : this->_materials)
        {
            this->_descriptorSets[material.first] = descriptorSets.at(i);
            material.second.setDescriptorSetID(i);
            ++i;
        }
    }

    void MaterialManager::recreatePipelines(const vk::Extent2D &extent, const vk::RenderPass &renderPass)
    {
        for (const auto &pipeline : this->_pipelines)
            this->createMaterialPipeline(extent, renderPass, pipeline.first, this->_materials.at(pipeline.first).hasTexture());
    }

    void MaterialManager::createDescriptorSetLayout()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings =
        {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment),
            vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment),
            vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
        };
        vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), static_cast<uint32_t>(bindings.size()), bindings.data());
        if (this->_descriptorSetLayout)
            this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
        this->_descriptorSetLayout = this->_device.createDescriptorSetLayout(layoutInfo, CUSTOM_ALLOCATOR);
    }

    void MaterialManager::createPipelineLayout()
    {
        vk::PushConstantRange pushConstantInfo(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4) * 2);
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo(vk::PipelineLayoutCreateFlags(), 1, &this->_descriptorSetLayout, 1, &pushConstantInfo);
        if (this->_pipelineLayout)
            this->_device.destroyPipelineLayout(this->_pipelineLayout, CUSTOM_ALLOCATOR);
        this->_pipelineLayout = this->_device.createPipelineLayout(pipelineLayoutInfo, CUSTOM_ALLOCATOR);
    }

    void MaterialManager::createMaterialPipeline(const vk::Extent2D &extent, const vk::RenderPass &renderPass, const size_t materialID, const bool texture)
    {
        std::vector<char> vertShaderCode;
        std::vector<char> fragShaderCode;
        if (texture)
        {
            vertShaderCode = Tools::readFile("shaders/materialTexture.vert.spv");
            fragShaderCode = Tools::readFile("shaders/materialTexture.frag.spv");
        }
        else
        {
            vertShaderCode = Tools::readFile("shaders/material.vert.spv");
            fragShaderCode = Tools::readFile("shaders/material.frag.spv");
        }
        vk::ShaderModule vertShaderModule = this->_device.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), vertShaderCode.size(), reinterpret_cast<uint32_t *>(vertShaderCode.data())), CUSTOM_ALLOCATOR);
        vk::ShaderModule fragShaderModule = this->_device.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), fragShaderCode.size(), reinterpret_cast<uint32_t *>(fragShaderCode.data())), CUSTOM_ALLOCATOR);
        vk::PipelineShaderStageCreateInfo shaderStages[] = { vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main"), vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main") };
        vk::VertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(), 1, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data());
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList, VK_FALSE);
        vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f);
        vk::Rect2D scissor(vk::Offset2D(), extent);
        vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);
        vk::PipelineRasterizationStateCreateInfo rasterizer(vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
        vk::PipelineMultisampleStateCreateInfo multisampling(vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE);
        vk::PipelineDepthStencilStateCreateInfo depthStencil(vk::PipelineDepthStencilStateCreateFlags(), VK_TRUE, VK_TRUE, vk::CompareOp::eLess);
        vk::PipelineColorBlendAttachmentState colorBlendAttachment(VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        vk::PipelineColorBlendStateCreateInfo colorBlending(vk::PipelineColorBlendStateCreateFlags(), VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment);
        vk::GraphicsPipelineCreateInfo pipelineInfo(vk::PipelineCreateFlags(), 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, &colorBlending, nullptr, this->_pipelineLayout, renderPass, 0, VK_NULL_HANDLE, -1);

        if (this->_pipelines.find(materialID) != this->_pipelines.end())
            this->_device.destroyPipeline(this->_pipelines.at(materialID), CUSTOM_ALLOCATOR);
        this->_pipelines[materialID] = this->_device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo, CUSTOM_ALLOCATOR);
        this->_device.destroyShaderModule(vertShaderModule, CUSTOM_ALLOCATOR);
        this->_device.destroyShaderModule(fragShaderModule, CUSTOM_ALLOCATOR);
    }
}
