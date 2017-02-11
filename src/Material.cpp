#include "Material.h"

namespace Dwarf
{
	Material::Material()
        : _texture(false)
	{
	}

	Material::~Material()
	{
	}

    void Material::setAmbient(const Color &color)
    {
        this->_data.Ka = color.getColor();
    }

    void Material::setDiffuse(const Color &color)
    {
        this->_data.Kd = color.getColor();
    }

    void Material::setSpecular(const Color &color)
    {
        this->_data.Ks = color.getColor();
    }

    void Material::setEmissive(const Color &color)
    {
        this->_data.Ke = color.getColor();
    }

    void Material::setTransmissionFilter(const Color &color)
    {
        this->_data.Tf = color.getColor();
    }

    void Material::setSpecularExponent(float value)
    {
        this->_data.Ns = value;
    }

    void Material::setRefraction(float value)
    {
        this->_data.Ni = value;
    }

    void Material::setDissolve(float value)
    {
        this->_data.d = value;
    }

    void Material::setTextureID(const size_t textureID)
    {
        this->_texture = true;
        this->_textureID = textureID;
    }

    void Material::setDescriptorSetID(const size_t descriptorSetID)
    {
        this->_descriptorSet = true;
        this->_descriptorSetID = descriptorSetID;
    }

    const MaterialData &Material::getData() const
    {
        return (this->_data);
    }

    bool Material::hasTexture() const
    {
        return (this->_texture);
    }
}
