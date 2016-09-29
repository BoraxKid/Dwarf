#ifndef TEXTURE_H_
#define TEXTURE_H_
#pragma once

#include <vulkan/vulkan.hpp>

#include "Tools.h"

namespace VkEngine
{
	class Texture
	{
	public:
		Texture(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string textureName, vk::ImageLayout imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal);
		virtual ~Texture();

		vk::DescriptorImageInfo &createTexture(vk::PhysicalDeviceMemoryProperties memProperties, vk::DescriptorPool &descriptorPool, vk::DescriptorSetLayout &descriptorSetLayout);

	private:
		const vk::Device &_device;
		const vk::CommandPool &_commandPool;
		const vk::Queue &_graphicsQueue;
		const std::string _textureName;

		vk::DeviceMemory _textureImageMemory;
		vk::Sampler _textureSampler;
		vk::Image _textureImage;
		vk::ImageLayout _textureImageLayout;
		vk::ImageView _textureImageView;
		vk::DescriptorImageInfo _imageInfo;
		uint32_t _width;
		uint32_t _height;
		uint32_t _mipLevels;
		uint32_t _layerCount;
	};
}

#endif // TEXTURE_H_
