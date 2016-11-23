#include "MaterialManager.h"
#include "Mesh.h"

namespace Dwarf
{
	MaterialManager::MaterialManager(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::RenderPass &renderPass, const vk::Extent2D &swapChainExtent)
        : _device(device), _graphicsQueue(graphicsQueue), _renderPass(renderPass), _swapChainExtent(swapChainExtent), _lastID(0)
	{
        this->createDescriptorSetLayout();
        this->createPipelineLayout();
        this->createMaterial("default");
	}

	MaterialManager::~MaterialManager()
	{
        std::map<const Material::ID, Material *>::iterator iterMaterials = this->_materials.begin();
        std::map<const Material::ID, Material *>::iterator iterMaterials2 = this->_materials.end();
        std::map<const Material::ID, vk::Pipeline>::iterator iterPipelines = this->_pipelines.begin();
        std::map<const Material::ID, vk::Pipeline>::iterator iterPipelines2 = this->_pipelines.end();

        while (iterMaterials != iterMaterials2)
        {
            delete (iterMaterials->second);
            ++iterMaterials;
        }
        while (iterPipelines != iterPipelines2)
        {
            this->_device.destroyPipeline(iterPipelines->second, CUSTOM_ALLOCATOR);
            ++iterPipelines;
        }

        this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
        this->_device.destroyDescriptorPool(this->_descriptorPool, CUSTOM_ALLOCATOR);
        this->_device.destroyPipelineLayout(this->_pipelineLayout, CUSTOM_ALLOCATOR);
    }

	bool MaterialManager::exist(const Material::ID materialID) const
	{
		return (this->_materials.find(materialID) != this->_materials.end());
	}

    bool MaterialManager::exist(const std::string &materialName) const
    {
        return (this->_materialsNames.find(materialName) != this->_materialsNames.end());
    }

	void MaterialManager::addMaterial(Material *material)
	{
		if (!material || this->exist(material->getID()))
			return; // TODO: add error handling
		this->_materials[material->getID()] = material;
	}

    Material *MaterialManager::getMaterial(const std::string &materialName) const
    {
        if (this->exist(materialName))
            return (this->_materials.at(this->_materialsNames.at(materialName)));
        return (nullptr);
    }

    Material *MaterialManager::createMaterial(const std::string &materialName)
    {
        if (this->_materialsNames.find(materialName) != this->_materialsNames.end())
            return (this->_materials.at(this->_materialsNames.at(materialName)));
        else
        {
            ++this->_lastID;
            this->createMaterialPipeline(this->_lastID);
            this->_materials[this->_lastID] = new Material(this->_device, this->_graphicsQueue, this->_pipelines.at(this->_lastID), this->_pipelineLayout, this->_lastID, materialName);
            this->_materialsNames[materialName] = this->_lastID;
            return (this->_materials.at(this->_lastID));
        }
    }

    void MaterialManager::createDescriptorPool()
    {
        uint32_t descriptorCount = static_cast<uint32_t>(this->_materials.size());
        std::vector<vk::DescriptorPoolSize> poolSizes = { vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, descriptorCount), vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, descriptorCount) };
        vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlags(), descriptorCount, static_cast<uint32_t>(poolSizes.size()), poolSizes.data());
        this->_descriptorPool = this->_device.createDescriptorPool(poolInfo, CUSTOM_ALLOCATOR);

        vk::DescriptorSetAllocateInfo allocInfo(this->_descriptorPool, 1, &this->_descriptorSetLayout);
        std::map<const Material::ID, Material *>::const_iterator iter = this->_materials.begin();
        std::map<const Material::ID, Material *>::const_iterator iterEnd = this->_materials.end();
        while (iter != iterEnd)
        {
            this->_descriptorSets[iter->first] = this->_device.allocateDescriptorSets(allocInfo).at(0);
            iter->second->setDescriptorSet(this->_descriptorSets[iter->first]);
            ++iter;
        }
    }

    void MaterialManager::createDescriptorSetLayout()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings =
        {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment),
            vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
        };
        vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), static_cast<uint32_t>(bindings.size()), bindings.data());
        if (this->_descriptorSetLayout != (vk::DescriptorSetLayout)VK_NULL_HANDLE)
            this->_device.destroyDescriptorSetLayout(this->_descriptorSetLayout, CUSTOM_ALLOCATOR);
        this->_descriptorSetLayout = this->_device.createDescriptorSetLayout(layoutInfo, CUSTOM_ALLOCATOR);
    }

    void MaterialManager::createPipelineLayout()
    {
        vk::PushConstantRange pushConstantInfo(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo(vk::PipelineLayoutCreateFlags(), 1, &this->_descriptorSetLayout, 1, &pushConstantInfo);
        if (this->_pipelineLayout != (vk::PipelineLayout)VK_NULL_HANDLE)
            this->_device.destroyPipelineLayout(this->_pipelineLayout, CUSTOM_ALLOCATOR);
        this->_pipelineLayout = this->_device.createPipelineLayout(pipelineLayoutInfo, CUSTOM_ALLOCATOR);
    }

    void MaterialManager::createMaterialPipeline(const Material::ID materialID)
    {
        std::vector<char> vertShaderCode;
        std::vector<char> fragShaderCode;
        if (materialID == 1)
        {
            vertShaderCode = Tools::readFile("shaders/shader.vert.spv");
            fragShaderCode = Tools::readFile("shaders/shader.frag.spv");
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
        vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(this->_swapChainExtent.width), static_cast<float>(this->_swapChainExtent.height), 0.0f, 1.0f);
        vk::Rect2D scissor(vk::Offset2D(), this->_swapChainExtent);
        vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);
        vk::PipelineRasterizationStateCreateInfo rasterizer(vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eFront, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
        vk::PipelineMultisampleStateCreateInfo multisampling(vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE);
        vk::PipelineDepthStencilStateCreateInfo depthStencil(vk::PipelineDepthStencilStateCreateFlags(), VK_TRUE, VK_TRUE, vk::CompareOp::eLess);
        vk::PipelineColorBlendAttachmentState colorBlendAttachment(VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        vk::PipelineColorBlendStateCreateInfo colorBlending(vk::PipelineColorBlendStateCreateFlags(), VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment);
        vk::GraphicsPipelineCreateInfo pipelineInfo(vk::PipelineCreateFlags(), 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, &colorBlending, nullptr, this->_pipelineLayout, this->_renderPass, 0, VK_NULL_HANDLE, -1);

        if (this->_pipelines.find(materialID) == this->_pipelines.end())
            this->_pipelines[materialID] = (vk::Pipeline)VK_NULL_HANDLE;
        if (this->_pipelines.at(materialID) != (vk::Pipeline)VK_NULL_HANDLE)
            this->_device.destroyPipeline(this->_pipelines.at(materialID), CUSTOM_ALLOCATOR);
        this->_pipelines.at(materialID) = this->_device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo, CUSTOM_ALLOCATOR);
        this->_device.destroyShaderModule(vertShaderModule, CUSTOM_ALLOCATOR);
        this->_device.destroyShaderModule(fragShaderModule, CUSTOM_ALLOCATOR);
    }

    bool MaterialManager::isSame(const Material::ID &leftMaterialID, Material *rightMaterial) const
	{
		if (!rightMaterial || this->_materials.find(leftMaterialID) == this->_materials.end())
			return (false);
		return (*this->_materials.at(leftMaterialID) == *rightMaterial);
	}

}
