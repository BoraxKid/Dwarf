#ifndef DWARF_MATERIAL_H_
#define DWARF_MATERIAL_H_
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
			if (this != &cpy)
			{
				type = cpy.type;
				if (type == COLOR)
					value.c = cpy.value.c;
				if (type == FLOAT)
					value.f = cpy.value.f;
				if (type == INT)
					value.i = cpy.value.i;
			}
			return (*this);
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

    struct MaterialUniformBuffer
    {
        glm::vec3 Ka; // ambient color (values between 0 and 1)
        glm::vec3 Kd; // diffuse color (values between 0 and 1)
        glm::vec3 Ks; // specular color (values between 0 and 1)
        glm::vec3 Tf; // transmittance || transmission filter (values between 0 and 1)
        glm::vec3 Ke; // emission || emissive (values between 0 and 1)
        float Ns; // shininess || specular exponent (values between 0 and 1000)
        float Ni; // ior - refraction (values between 0.001 and 10)
        float d; // (or Tr where Tr = 1 - d) dissolve || transparency (values between 0 and 1 with 0.0 being fully dissolved - completely transparent and 1.0 fully opaque)
        int illum; // illumination model - ignored for now
    };

	class Material
	{
	public:
        typedef int ID;
		Material(const vk::Device &device, const vk::Queue &graphicsQueue, const vk::Pipeline &pipeline, const vk::PipelineLayout &pipelineLayout, ID id, const std::string &name);
		virtual ~Material();
        void buildDescriptorSet(const vk::Buffer &buffer, const vk::DeviceSize &uniformBufferOffset, const vk::PhysicalDeviceMemoryProperties &memProperties, const vk::DescriptorBufferInfo &lightBufferInfo);
		bool isSame(const Material &material) const;
        ID getID() const;
        const std::string &getName() const;
        const vk::Pipeline &getPipeline() const;
        const vk::PipelineLayout &getPipelineLayout() const;
        const vk::DescriptorSet &getDescriptorSet() const;
        const MaterialUniformBuffer &getUniformBuffer() const;
        bool hasDiffuseTexture() const;
        void setCommandPool(vk::CommandPool *commandPool);
        void setDescriptorSet(vk::DescriptorSet descriptorSet);
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
		const vk::Queue &_graphicsQueue;
		const vk::Pipeline &_pipeline;
        const vk::PipelineLayout &_pipelineLayout;
        vk::CommandPool *_commandPool;
		vk::DescriptorSet _descriptorSet;
        const ID _id;
        const std::string _name;
        MaterialUniformBuffer _uniformBuffer;
		std::map<const MaterialType, Value> _values;
		std::map<const MaterialType, Texture *> _textures;
	};

	bool operator==(const Material &lhs, const Material &rhs);
	bool operator!=(const Material &lhs, const Material &rhs);
}

#endif // DWARF_MATERIAL_H_
