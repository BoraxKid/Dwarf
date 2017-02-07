#include "Color.h"

namespace Dwarf
{
	Color::Color(float red, float green, float blue, float alpha)
		: _valuesAlpha(red, green, blue, alpha), _values(_valuesAlpha)
	{
	}

	Color::~Color()
	{
	}

	void Color::setRed(float red)
	{
		this->_valuesAlpha.r = red;
        this->_values.r = red;
	}

	void Color::setGreen(float green)
	{
		this->_valuesAlpha.g = green;
        this->_values.g = green;
	}

	void Color::setBlue(float blue)
	{
		this->_valuesAlpha.b = blue;
        this->_values.b = blue;
	}

	void Color::setAlpha(float alpha)
	{
        this->_valuesAlpha.a = alpha;
	}

	void Color::setValues(float red, float green, float blue)
	{
        this->_valuesAlpha.r = red;
        this->_valuesAlpha.g = green;
        this->_valuesAlpha.b = blue;
        this->_values.r = red;
        this->_values.g = green;
        this->_values.b = blue;
	}

	void Color::setValues(float red, float green, float blue, float alpha)
	{
		this->_valuesAlpha = glm::vec4(red, green, blue, alpha);
        this->_values = glm::vec3(red, green, blue);
	}

	void Color::setValues(glm::vec3 values)
	{
		this->_valuesAlpha.r = values.r;
		this->_valuesAlpha.g = values.g;
		this->_valuesAlpha.b = values.b;
        this->_values = values;
	}

	void Color::setValues(glm::vec4 values)
	{
		this->_valuesAlpha = values;
        this->_values = values;
	}

    const glm::vec3 &Color::getColor() const
    {
        return (this->_values);
    }

    const glm::vec4 &Color::getColorWithAlpha() const
    {
        return (this->_valuesAlpha);
    }

	bool operator==(const Color &lhs, const Color &rhs)
	{
		return (lhs.getColorWithAlpha() == rhs.getColorWithAlpha());
	}

	bool operator!=(const Color &lhs, const Color &rhs)
	{
		return (lhs.getColorWithAlpha() != rhs.getColorWithAlpha());
	}
}
