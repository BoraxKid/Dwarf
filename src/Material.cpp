#include "Material.h"

namespace Dwarf
{
	Material::Material(size_t id)
        : _id(id)
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

    size_t Material::getID() const
    {
        return (this->_id);
    }
}
