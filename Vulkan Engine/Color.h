#ifndef COLOR_H_
#define COLOR_H_
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Dwarf
{
	class Color
	{
	public:
		Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f);
		virtual ~Color();
		void setRed(float red);
		void setGreen(float green);
		void setBlue(float blue);
		void setAlpha(float alpha);
		void setValues(float red, float green, float blue);
		void setValues(float red, float green, float blue, float alpha);
		void setValues(glm::vec3 values);
		void setValues(glm::vec4 values);
		const glm::vec4 &getColor() const;

	private:
		glm::vec4 _values;
	};
}

#endif // COLOR_H_
