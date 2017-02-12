#include "Material.h"

namespace Dwarf
{
	Material::Material()
        : _texture(false), _uniformBufferID(0), _uniformBufferOffset(0)
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

    void Material::setUniformBufferID(const size_t uniformBufferID)
    {
        this->_uniformBufferID = uniformBufferID;
    }

    void Material::setUniformBufferOffset(const vk::DeviceSize uniformBufferOffset)
    {
        this->_uniformBufferOffset = uniformBufferOffset;
    }

    size_t Material::getTextureID() const
    {
        return (this->_textureID);
    }

    size_t Material::getDescriptorSetID() const
    {
        return (this->_descriptorSetID);
    }

    size_t Material::getUniformBufferID() const
    {
        return (this->_uniformBufferID);
    }

    vk::DeviceSize Material::getUniformBufferOffset() const
    {
        return (this->_uniformBufferOffset);
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
