#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Dwarf
{
	Texture::Texture(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::string textureName, vk::ImageLayout imageLayout)
		: _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue), _textureName(textureName), _textureImageLayout(imageLayout)
	{
	}

	Texture::~Texture()
	{
		this->_device.destroySampler(this->_textureSampler, CUSTOM_ALLOCATOR);
		this->_device.destroyImageView(this->_textureImageView, CUSTOM_ALLOCATOR);
		this->_device.freeMemory(this->_textureImageMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyImage(this->_textureImage, CUSTOM_ALLOCATOR);
	}

	vk::DescriptorImageInfo &Texture::createTexture(vk::PhysicalDeviceMemoryProperties memProperties, vk::DescriptorPool &descriptorPool, vk::DescriptorSetLayout &descriptorSetLayout)
	{
		int textureWidth;
		int textureHeight;
		int textureChannels;
		stbi_uc *pixels = stbi_load(this->_textureName.c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
		if (!pixels)
			Tools::exitOnError("Failed to load texture " + this->_textureName + "file");
		this->_width = static_cast<uint32_t>(textureWidth);
		this->_height = static_cast<uint32_t>(textureHeight);
		vk::DeviceSize imageSize = this->_width * this->_height * STBI_rgb_alpha;
		vk::Image stagingImage;
		vk::DeviceMemory stagingImageMemory;
		Tools::createImage(this->_device, memProperties, this->_width, this->_height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingImage, stagingImageMemory);
		void *data = this->_device.mapMemory(stagingImageMemory, 0, imageSize);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		this->_device.unmapMemory(stagingImageMemory);
		stbi_image_free(pixels);
		Tools::createImage(this->_device, memProperties, this->_width, this->_height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, this->_textureImage, this->_textureImageMemory);
		Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPool, stagingImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal);
		Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPool, this->_textureImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);
		Tools::copyImage(this->_device, this->_graphicsQueue, this->_commandPool, stagingImage, this->_textureImage, this->_width, this->_height);
		Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPool, this->_textureImage, vk::ImageLayout::eTransferDstOptimal, this->_textureImageLayout);
		this->_device.freeMemory(stagingImageMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyImage(stagingImage, CUSTOM_ALLOCATOR);
		Tools::createImageView(this->_device, this->_textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, this->_textureImageView);
		vk::SamplerCreateInfo samplerInfo(vk::SamplerCreateFlags(), vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f, VK_TRUE, 16, VK_FALSE, vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
		this->_textureSampler = this->_device.createSampler(samplerInfo, CUSTOM_ALLOCATOR);

		/*vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &descriptorSetLayout);
		this->_descriptorSet = this->_device.allocateDescriptorSets(allocInfo).at(0);
		vk::DescriptorBufferInfo bufferInfo(this->_buffer, this->_uniformBufferOffset, sizeof(UniformBufferObject));*/
		this->_imageInfo = vk::DescriptorImageInfo(this->_textureSampler, this->_textureImageView, this->_textureImageLayout);
		return (this->_imageInfo);
		/*std::array<vk::WriteDescriptorSet, 2> descriptorWrites = { vk::WriteDescriptorSet(this->_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo), vk::WriteDescriptorSet(this->_descriptorSet, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo) };
		this->_device.updateDescriptorSets(descriptorWrites, nullptr);*/
	}
}
