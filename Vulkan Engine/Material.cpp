#include "Material.h"

namespace VkEngine
{
	Material::Material(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string &name)
		: _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue), _name(name), _ambientTexture(nullptr), _diffuseTexture(nullptr), _specularTexture(nullptr), _specularHighlightTexture(nullptr), _bumpTexture(nullptr), _displacementTexture(nullptr), _alphaTexture(nullptr), _roughnessTexture(nullptr), _metallicTexture(nullptr), _sheenTexture(nullptr), _emissiveTexture(nullptr), _normalTexture(nullptr)
	{
	}

	Material::~Material()
	{
	}

	void Material::setAmbient(Color value)
	{
		this->_ambient = value;
	}

	void Material::setDiffuse(Color value)
	{
		this->_diffuse = value;
	}

	void Material::setSpecular(Color value)
	{
		this->_specular = value;
	}

	void Material::setTransmittance(Color value)
	{
		this->_transmittance = value;
	}

	void Material::setEmission(Color value)
	{
		this->_emission = value;
	}

	void Material::setShininess(float value)
	{
		this->_shininess = value;
	}

	void Material::setIor(float value)
	{
		this->_ior = value;
	}

	void Material::setDissolve(float value)
	{
		this->_dissolve = value;
	}

	void Material::setIllum(float value)
	{
		this->_illum = value;
	}

	void Material::setRoughness(float value)
	{
		this->_roughness = value;
	}

	void Material::setMetallic(float value)
	{
		this->_metallic = value;
	}

	void Material::setSheen(float value)
	{
		this->_sheen = value;
	}

	void Material::setClearcoatThickness(float value)
	{
		this->_clearcoatThickness = value;
	}

	void Material::setClearcoatRoughness(float value)
	{
		this->_clearcoatRoughness = value;
	}

	void Material::setAnisotropy(float value)
	{
		this->_anisotropy = value;
	}

	void Material::setAnisotropyRotation(float value)
	{
		this->_anisotropyRotation = value;
	}

	void Material::createAmbientTexture(const std::string &textureName)
	{
		this->_ambientTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createDiffuseTexture(const std::string &textureName)
	{
		this->_diffuseTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSpecularTexture(const std::string &textureName)
	{
		this->_specularTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSpecularHighlightTexture(const std::string &textureName)
	{
		this->_specularHighlightTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createBumpTexture(const std::string &textureName)
	{
		this->_bumpTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createDisplacementTexture(const std::string &textureName)
	{
		this->_displacementTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createAlphaTexture(const std::string &textureName)
	{
		this->_alphaTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createRoughnessTexture(const std::string &textureName)
	{
		this->_roughnessTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createMetallicTexture(const std::string &textureName)
	{
		this->_metallicTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createSheenTexture(const std::string &textureName)
	{
		this->_sheenTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createEmissiveTexture(const std::string &textureName)
	{
		this->_emissiveTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}

	void Material::createNormalTexture(const std::string &textureName)
	{
		this->_normalTexture = new Texture(this->_device, this->_commandPool, this->_graphicsQueue, textureName);
	}
}
