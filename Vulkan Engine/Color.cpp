#include "Color.h"

namespace Dwarf
{
	Color::Color(float red, float green, float blue, float alpha)
		: _values(red, green, blue, alpha)
	{
	}

	Color::~Color()
	{
	}

	void Color::setRed(float red)
	{
		this->_values.r = red;
	}

	void Color::setGreen(float green)
	{
		this->_values.g = green;
	}

	void Color::setBlue(float blue)
	{
		this->_values.b = blue;
	}

	void Color::setAlpha(float alpha)
	{
		this->_values.a = alpha;
	}

	void Color::setValues(float red, float green, float blue)
	{
		this->_values.r = red;
		this->_values.g = green;
		this->_values.b = blue;
	}

	void Color::setValues(float red, float green, float blue, float alpha)
	{
		this->_values = glm::vec4(red, green, blue, alpha);
	}

	void Color::setValues(glm::vec3 values)
	{
		this->_values.r = values.r;
		this->_values.g = values.g;
		this->_values.b = values.b;
	}

	void Color::setValues(glm::vec4 values)
	{
		this->_values = values;
	}

	const glm::vec4 &Color::getColor() const
	{
		return (this->_values);
	}

	bool operator==(const Color &lhs, const Color &rhs)
	{
		return (lhs.getColor() == rhs.getColor());
	}

	bool operator!=(const Color &lhs, const Color &rhs)
	{
		return (lhs.getColor() != rhs.getColor());
	}
}
