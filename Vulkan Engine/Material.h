#ifndef MATERIAL_H_
#define MATERIAL_H_
#pragma once

#include "Color.h"
#include "Texture.h"

namespace VkEngine
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

	class Material
	{
	public:
		Material(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string &name);
		virtual ~Material();
		void setAmbient(Color value);
		void setDiffuse(Color value);
		void setSpecular(Color value);
		void setTransmittance(Color value);
		void setEmission(Color value);
		void setShininess(float value);
		void setIor(float value);
		void setDissolve(float value);
		void setIllum(float value);
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
		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;
		vk::Pipeline _pipeline;
		vk::DescriptorSet _descriptorSet;
		const std::string &_name;
		Color _ambient;
		Color _diffuse;
		Color _specular;
		Color _transmittance;
		Color _emission;
		float _shininess;
		float _ior;
		float _dissolve;
		float _illum;
		float _roughness;
		float _metallic;
		float _sheen;
		float _clearcoatThickness;
		float _clearcoatRoughness;
		float _anisotropy;
		float _anisotropyRotation;
		std::vector<Texture *> _textures;
		Texture *_ambientTexture;
		Texture *_diffuseTexture;
		Texture *_specularTexture;
		Texture *_specularHighlightTexture;
		Texture *_bumpTexture;
		Texture *_displacementTexture;
		Texture *_alphaTexture;
		Texture *_roughnessTexture;
		Texture *_metallicTexture;
		Texture *_sheenTexture;
		Texture *_emissiveTexture;
		Texture *_normalTexture;
	};
}

#endif // MATERIAL_H_
