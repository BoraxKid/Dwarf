#include "Material.h"

namespace Dwarf
{
	Material::Material(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string &name)
		: _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue), _name(name), _ambientTexture(nullptr), _diffuseTexture(nullptr), _specularTexture(nullptr), _specularHighlightTexture(nullptr), _bumpTexture(nullptr), _displacementTexture(nullptr), _alphaTexture(nullptr), _roughnessTexture(nullptr), _metallicTexture(nullptr), _sheenTexture(nullptr), _emissiveTexture(nullptr), _normalTexture(nullptr)
	{
	}

	Material::~Material()
	{
	}

	bool Material::isSame(const Material &material) const
	{
		if (this->_name != material._name || this->_ambient != material._ambient || this->_diffuse != material._diffuse || this->_specular != material._specular || this->_transmittance != material._transmittance || this->_emission != material._emission || this->_shininess != material._shininess || this->_ior != material._ior || this->_dissolve != material._dissolve || this->_illum != material._illum || this->_roughness != material._roughness || this->_metallic != material._metallic || this->_sheen != material._sheen || this->_clearcoatThickness != material._clearcoatThickness || this->_clearcoatRoughness != material._clearcoatRoughness || this->_anisotropy != material._anisotropy || this->_anisotropyRotation != material._anisotropyRotation)
			return (false);
		if (this->_ambientTexture != material._ambientTexture || this->_diffuseTexture != material._diffuseTexture || this->_specularTexture != material._specularTexture || this->_specularHighlightTexture != material._specularHighlightTexture || this->_bumpTexture != material._bumpTexture || this->_displacementTexture != material._displacementTexture || this->_alphaTexture != material._alphaTexture || this->_roughnessTexture != material._roughnessTexture || this->_metallicTexture != material._metallicTexture || this->_sheenTexture != material._sheenTexture || this->_emissiveTexture != material._emissiveTexture || this->_normalTexture != material._normalTexture)
			return (false);
		return (true);
	}

	const std::string &Material::getName() const
	{
		return (this->_name);
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

	void Material::setIllum(int value)
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

	bool operator==(const Material &lhs, const Material &rhs)
	{
		return (lhs.isSame(rhs));
	}

	bool operator!=(const Material &lhs, const Material &rhs)
	{
		return (!lhs.isSame(rhs));
	}
}
