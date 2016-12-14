#include "Material.h"

namespace Dwarf
{
	Material::Material(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::Pipeline &pipeline, const vk::PipelineLayout &pipelineLayout, Material::ID id, const std::string &name)
		: _device(device), _graphicsQueue(graphicsQueue), _pipeline(pipeline), _pipelineLayout(pipelineLayout), _id(id), _name(name)
	{
        this->init();
	}

	Material::~Material()
	{
	}

    void Material::buildDescriptorSet(const vk::Buffer &buffer, const vk::DeviceSize &uniformBufferOffset, const vk::PhysicalDeviceMemoryProperties &memProperties)
    {
        //vk::DescriptorImageInfo &imageInfo = this->_texture->createTexture(memProperties, descriptorPool, descriptorSetLayout);

        vk::DescriptorBufferInfo bufferInfo(buffer, uniformBufferOffset, sizeof(MaterialUniformBuffer));
        std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(this->_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo)/*, vk::WriteDescriptorSet(this->_descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo)*/ };
        std::map<const MaterialType, Texture *>::iterator iter = this->_textures.begin();
        std::map<const MaterialType, Texture *>::iterator iter2 = this->_textures.end();
        while (iter != iter2)
        {
            descriptorWrites.push_back(vk::WriteDescriptorSet(this->_descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &iter->second->createTexture(memProperties)));
            ++iter;
        }
        this->_device.updateDescriptorSets(descriptorWrites, nullptr);
    }

	bool Material::isSame(const Material &material) const
	{
		if (this->_id != material._id || this->_values.size() != material._values.size() || !std::equal(this->_values.begin(), this->_values.end(), material._values.begin()) || this->_textures.size() != material._textures.size() || !std::equal(this->_textures.begin(), this->_textures.end(), material._textures.begin()))
			return (false);
		return (true);
	}

    Material::ID Material::getID() const
    {
        return (this->_id);
    }

    const std::string &Material::getName() const
    {
        return (this->_name);
    }

    const vk::Pipeline &Material::getPipeline() const
    {
        return (this->_pipeline);
    }

    const vk::PipelineLayout &Material::getPipelineLayout() const
    {
        return (this->_pipelineLayout);
    }

    const vk::DescriptorSet &Material::getDescriptorSet() const
    {
        return (this->_descriptorSet);
    }

    const MaterialUniformBuffer &Material::getUniformBuffer() const
    {
        return (this->_mub);
    }

    bool Material::hasDiffuseTexture() const
    {
        return (this->_textures.find(DIFFUSE) != this->_textures.end());
    }

    void Material::setCommandPool(vk::CommandPool *commandPool)
    {
        this->_commandPool = commandPool;
        std::map<const MaterialType, Texture *>::iterator iter = this->_textures.begin();
        std::map<const MaterialType, Texture *>::iterator iter2 = this->_textures.end();
        while (iter != iter2)
        {
            iter->second->setCommandPool(commandPool);
            ++iter;
        }
    }

    void Material::setDescriptorSet(vk::DescriptorSet descriptorSet)
    {
        this->_descriptorSet = descriptorSet;
    }

	void Material::setAmbient(Color value)
	{
		this->_values[AMBIENT].value.c = value;
        this->_mub.Ka = value.getColor();
	}

	void Material::setDiffuse(Color value)
	{
		this->_values[DIFFUSE].value.c = value;
        this->_mub.Kd = value.getColor();
	}

	void Material::setSpecular(Color value)
	{
		this->_values[SPECULAR].value.c = value;
        this->_mub.Ks = value.getColor();
	}

	void Material::setTransmittance(Color value)
	{
		this->_values[TRANSMITTANCE].value.c = value;
        this->_mub.Tf = value.getColor();
	}

	void Material::setEmission(Color value)
	{
		this->_values[EMISSION].value.c = value;
        this->_mub.Ke = value.getColor();
	}

	void Material::setShininess(float value)
	{
		this->_values[SHININESS].value.f = value;
        this->_mub.Ns = value;
	}

	void Material::setIor(float value)
	{
		this->_values[IOR].value.f = value;
        this->_mub.Ni = value;
	}

	void Material::setDissolve(float value)
	{
		this->_values[DISSOLVE].value.f = value;
        this->_mub.d = value;
	}

	void Material::setIllum(int value)
	{
		this->_values[ILLUM].value.i = value;
        this->_mub.illum = value;
	}

	void Material::setRoughness(float value)
	{
		this->_values[ROUGHNESS].value.f = value;
	}

	void Material::setMetallic(float value)
	{
		this->_values[METALLIC].value.f = value;
	}

	void Material::setSheen(float value)
	{
		this->_values[SHEEN].value.f = value;
	}

	void Material::setClearcoatThickness(float value)
	{
		this->_values[CLEARCOAT_THICKNESS].value.f = value;
	}

	void Material::setClearcoatRoughness(float value)
	{
		this->_values[CLEARCOAT_ROUGHNESS].value.f = value;
	}

	void Material::setAnisotropy(float value)
	{
		this->_values[ANISOTROPY].value.f = value;
	}

	void Material::setAnisotropyRotation(float value)
	{
		this->_values[ANISOTROPY_ROTATION].value.f = value;
	}

	void Material::createAmbientTexture(const std::string &textureName)
	{
		if (this->_textures.find(AMBIENT) != this->_textures.end())
			delete (this->_textures.at(AMBIENT));
		this->_textures[AMBIENT] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createDiffuseTexture(const std::string &textureName)
	{
		if (this->_textures.find(DIFFUSE) != this->_textures.end())
			delete (this->_textures.at(DIFFUSE));
		this->_textures[DIFFUSE] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSpecularTexture(const std::string &textureName)
	{
		if (this->_textures.find(SPECULAR) != this->_textures.end())
			delete (this->_textures.at(SPECULAR));
		this->_textures[SPECULAR] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSpecularHighlightTexture(const std::string &textureName)
	{
		if (this->_textures.find(SPECULAR_HIGHLIGHT) != this->_textures.end())
			delete (this->_textures.at(SPECULAR_HIGHLIGHT));
		this->_textures[SPECULAR_HIGHLIGHT] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createBumpTexture(const std::string &textureName)
	{
		if (this->_textures.find(BUMP) != this->_textures.end())
			delete (this->_textures.at(BUMP));
		this->_textures[BUMP] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createDisplacementTexture(const std::string &textureName)
	{
		if (this->_textures.find(DISPLACEMENT) != this->_textures.end())
			delete (this->_textures.at(DISPLACEMENT));
		this->_textures[DISPLACEMENT] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createAlphaTexture(const std::string &textureName)
	{
		if (this->_textures.find(ALPHA) != this->_textures.end())
			delete (this->_textures.at(ALPHA));
		this->_textures[ALPHA] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createRoughnessTexture(const std::string &textureName)
	{
		if (this->_textures.find(ROUGHNESS) != this->_textures.end())
			delete (this->_textures.at(ROUGHNESS));
		this->_textures[ROUGHNESS] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createMetallicTexture(const std::string &textureName)
	{
		if (this->_textures.find(METALLIC) != this->_textures.end())
			delete (this->_textures.at(METALLIC));
		this->_textures[METALLIC] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSheenTexture(const std::string &textureName)
	{
		if (this->_textures.find(SHEEN) != this->_textures.end())
			delete (this->_textures.at(SHEEN));
		this->_textures[SHEEN] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createEmissiveTexture(const std::string &textureName)
	{
		if (this->_textures.find(EMISSIVE) != this->_textures.end())
			delete (this->_textures.at(EMISSIVE));
		this->_textures[EMISSIVE] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createNormalTexture(const std::string &textureName)
	{
		if (this->_textures.find(NORMAL) != this->_textures.end())
			delete (this->_textures.at(NORMAL));
		this->_textures[NORMAL] = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::init()
	{
		this->_values[AMBIENT] = Value(ValueType::COLOR, Color());
		this->_values[DIFFUSE] = Value(ValueType::COLOR, Color());
		this->_values[SPECULAR] = Value(ValueType::COLOR, Color());
		this->_values[TRANSMITTANCE] = Value(ValueType::COLOR, Color());
		this->_values[EMISSION] = Value(ValueType::COLOR, Color());
		this->_values[SHININESS] = Value(ValueType::FLOAT, 1.0f);
		this->_values[IOR] = Value(ValueType::FLOAT, 1.0f);
		this->_values[DISSOLVE] = Value(ValueType::FLOAT, 1.0f);
		this->_values[ILLUM] = Value(ValueType::INT, 0);
		this->_values[ROUGHNESS] = Value(ValueType::FLOAT, 1.0f);
		this->_values[METALLIC] = Value(ValueType::FLOAT, 1.0f);
		this->_values[SHEEN] = Value(ValueType::FLOAT, 1.0f);
		this->_values[CLEARCOAT_THICKNESS] = Value(ValueType::FLOAT, 1.0f);
		this->_values[CLEARCOAT_ROUGHNESS] = Value(ValueType::FLOAT, 1.0f);
		this->_values[ANISOTROPY] = Value(ValueType::FLOAT, 1.0f);
		this->_values[ANISOTROPY_ROTATION] = Value(ValueType::FLOAT, 1.0f);
	}

	bool operator==(const Value &lhs, const Value &rhs)
	{
		return (lhs.isSame(rhs));
	}

	bool operator!=(const Value &lhs, const Value &rhs)
	{
		return (!lhs.isSame(rhs));
	}

	bool operator==(const Material &lhs, const Material &rhs)
	{
		return (lhs.isSame(rhs));
	}

	bool operator!=(const Material &lhs, const Material &rhs)
	{
		return (!lhs.isSame(rhs));
	}
}
