#include "MaterialManager.h"

namespace Dwarf
{
	MaterialManager::MaterialManager(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue)
        : _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue), _lastID(0)
	{
	}

	MaterialManager::~MaterialManager()
	{
	}

	bool MaterialManager::exist(const Material::ID materialID) const
	{
		return (this->_materials.find(materialID) != this->_materials.end());
	}

	void MaterialManager::addMaterial(Material *material)
	{
		if (!material || this->exist(material->getID()))
			return; // TODO: add error handling
		this->_materials[material->getID()] = material;
	}

    Material *MaterialManager::createMaterial(const std::string &materialName)
    {
        ++this->_lastID;
        this->_materials[this->_lastID] = new Material(this->_device, this->_commandPool, this->_graphicsQueue, this->_lastID);
        if (this->_materialsNames.find(materialName) != this->_materialsNames.end())
            this->_materialsNames[materialName + "_"] = this->_lastID;
        else
            this->_materialsNames[materialName] = this->_lastID;
        return nullptr;
    }

    void MaterialManager::createDescriptorSetLayout()
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings =
        {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex),
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

    bool MaterialManager::isSame(const Material::ID &leftMaterialID, Material *rightMaterial) const
	{
		if (!rightMaterial || this->_materials.find(leftMaterialID) == this->_materials.end())
			return (false);
		return (*this->_materials.at(leftMaterialID) == *rightMaterial);
	}

}
