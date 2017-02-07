#ifndef DWARF_COLOR_H_
#define DWARF_COLOR_H_
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Dwarf
{
	class Color
	{
	public:
		Color(float red = 1.0f, float green = 0.0f, float blue = 1.0f, float alpha = 1.0f);
		virtual ~Color();

		void setRed(float red);
		void setGreen(float green);
		void setBlue(float blue);
		void setAlpha(float alpha);
		void setValues(float red, float green, float blue);
		void setValues(float red, float green, float blue, float alpha);
		void setValues(glm::vec3 values);
		void setValues(glm::vec4 values);
        const glm::vec3 &getColor() const;
        const glm::vec4 &getColorWithAlpha() const;

	private:
        glm::vec3 _values;
		glm::vec4 _valuesAlpha;
	};

	bool operator==(const Color &lhs, const Color &rhs);
	bool operator!=(const Color &lhs, const Color &rhs);
}

#endif // DWARF_COLOR_H_
