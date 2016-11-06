#include "Material.h"

namespace Dwarf
{
	Material::Material(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Material::ID id)
		: _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue), _id(id)
	{
	}

	Material::~Material()
	{
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

	void Material::setAmbient(Color value)
	{
		this->_values.at(AMBIENT).value.c = value;
	}

	void Material::setDiffuse(Color value)
	{
		this->_values.at(DIFFUSE).value.c = value;
	}

	void Material::setSpecular(Color value)
	{
		this->_values.at(SPECULAR).value.c = value;
	}

	void Material::setTransmittance(Color value)
	{
		this->_values.at(TRANSMITTANCE).value.c = value;
	}

	void Material::setEmission(Color value)
	{
		this->_values.at(EMISSION).value.c = value;
	}

	void Material::setShininess(float value)
	{
		this->_values.at(SHININESS).value.f = value;
	}

	void Material::setIor(float value)
	{
		this->_values.at(IOR).value.f = value;
	}

	void Material::setDissolve(float value)
	{
		this->_values.at(DISSOLVE).value.f = value;
	}

	void Material::setIllum(int value)
	{
		this->_values.at(ILLUM).value.i = value;
	}

	void Material::setRoughness(float value)
	{
		this->_values.at(ROUGHNESS).value.f = value;
	}

	void Material::setMetallic(float value)
	{
		this->_values.at(METALLIC).value.f = value;
	}

	void Material::setSheen(float value)
	{
		this->_values.at(SHEEN).value.f = value;
	}

	void Material::setClearcoatThickness(float value)
	{
		this->_values.at(CLEARCOAT_THICKNESS).value.f = value;
	}

	void Material::setClearcoatRoughness(float value)
	{
		this->_values.at(CLEARCOAT_ROUGHNESS).value.f = value;
	}

	void Material::setAnisotropy(float value)
	{
		this->_values.at(ANISOTROPY).value.f = value;
	}

	void Material::setAnisotropyRotation(float value)
	{
		this->_values.at(ANISOTROPY_ROTATION).value.f = value;
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
