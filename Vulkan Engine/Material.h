#ifndef MATERIAL_H_
#define MATERIAL_H_
#pragma once

#include <map>

#include "Color.h"
#include "Texture.h"

namespace Dwarf
{
	enum MaterialType
	{
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		SPECULAR_HIGHLIGHT,
		TRANSMITTANCE,
		EMISSION,
		SHININESS,
		IOR,
		DISSOLVE,
		ILLUM,
		CLEARCOAT_THICKNESS,
		CLEARCOAT_ROUGHNESS,
		ANISOTROPY,
		ANISOTROPY_ROTATION,
		BUMP,
		DISPLACEMENT,
		ALPHA,
		ROUGHNESS,
		METALLIC,
		SHEEN,
		EMISSIVE,
		NORMAL
	};

	enum ValueType
	{
		COLOR,
		FLOAT,
		INT
	};

	union UnionValue
	{
		UnionValue(Color value) : c(value) {};
		UnionValue(float value) : f(value) {};
		UnionValue(int value) : i(value) {};
		~UnionValue() {};
		Color c;
		float f;
		int i;
	};

	struct Value
	{
		Value() : type(INT), value(0) {}
		Value(ValueType t, Color c) : type(t), value(c) {}
		Value &operator=(const Value &cpy)
		{
			type = cpy.type;
			if (type == COLOR)
				value.c = cpy.value.c;
			if (type == FLOAT)
				value.f = cpy.value.f;
			if (type == INT)
				value.i = cpy.value.i;
		}
		bool isSame(const Value &cpy) const
		{
			if (type != cpy.type)
				return (false);
			if (type == COLOR && value.c != cpy.value.c)
				return (false);
			if (type == FLOAT && value.f != cpy.value.f)
				return (false);
			if (type == INT && value.i != cpy.value.i)
				return (false);
			return (true);
		}

		ValueType type;
		UnionValue value;
	};

	bool operator==(const Value &lhs, const Value &rhs);
	bool operator!=(const Value &lhs, const Value &rhs);

	class Material
	{
	public:
		Material(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string &name);
		virtual ~Material();
		bool isSame(const Material &material) const;
		const std::string &getName() const;
		void setAmbient(Color value);
		void setDiffuse(Color value);
		void setSpecular(Color value);
		void setTransmittance(Color value);
		void setEmission(Color value);
		void setShininess(float value);
		void setIor(float value);
		void setDissolve(float value);
		void setIllum(int value);
		void setRoughness(float value);
		void setMetallic(float value);
		void setSheen(float value);
		void setClearcoatThickness(float value);
		void setClearcoatRoughness(float value);
		void setAnisotropy(float value);
		void setAnisotropyRotation(float value);
		void createAmbientTexture(const std::string &textureName);
		void createDiffuseTexture(const std::string &textureName);
		void createSpecularTexture(const std::string &textureName);
		void createSpecularHighlightTexture(const std::string &textureName);
		void createBumpTexture(const std::string &textureName);
		void createDisplacementTexture(const std::string &textureName);
		void createAlphaTexture(const std::string &textureName);
		void createRoughnessTexture(const std::string &textureName);
		void createMetallicTexture(const std::string &textureName);
		void createSheenTexture(const std::string &textureName);
		void createEmissiveTexture(const std::string &textureName);
		void createNormalTexture(const std::string &textureName);

	private:
		void init();

		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;
		vk::Pipeline _pipeline;
		vk::DescriptorSet _descriptorSet;
		const std::string &_name;
		std::map<const MaterialType, Value> _values;
		std::map<const MaterialType, Texture *> _textures;
	};

	bool operator==(const Material &lhs, const Material &rhs);
	bool operator!=(const Material &lhs, const Material &rhs);
}

#endif // MATERIAL_H_
