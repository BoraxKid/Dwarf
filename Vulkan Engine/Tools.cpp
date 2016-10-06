#include "Tools.h"

namespace Dwarf
{
	namespace Tools
	{
		bool checkValidationLayerSupport(const std::vector<const char *> validationLayers)
		{
			std::vector<const char *>::const_iterator iter = validationLayers.begin();
			std::vector<const char *>::const_iterator iter2 = validationLayers.end();
			std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
			std::vector<vk::LayerProperties>::const_iterator iter3;
			std::vector<vk::LayerProperties>::const_iterator iter4 = availableLayers.end();
			bool layerFound;

			while (iter != iter2)
			{
				iter3 = availableLayers.begin();
				layerFound = false;
				while (iter3 != iter4)
				{
					if (strcmp(*iter, iter3->layerName) == 0)
					{
						layerFound = true;
						break;
					}
					++iter3;
				}
				if (!layerFound)
					return (false);
				++iter;
			}
			return (true);
		}

		std::vector<const char *> getRequiredExtensions(bool validationLayerEnabled)
		{
			std::vector<const char *> extensions;
			unsigned int glfwExtensionCount = 0;
			const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			unsigned int i = 0;

			while (i < glfwExtensionCount)
			{
				extensions.push_back(glfwExtensions[i]);
				++i;
			}
			if (validationLayerEnabled)
				extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			return (extensions);
		}

		VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
		{
			PFN_vkCreateDebugReportCallbackEXT func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

			if (func != nullptr)
				return (func(instance, pCreateInfo, pAllocator, pCallback));
			else
				return (VK_ERROR_EXTENSION_NOT_PRESENT);
		}

		void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks *pAllocator)
		{
			PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
			if (func != nullptr)
				func(instance, callback, pAllocator);
		}

		uint32_t getMemoryType(vk::PhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
		{
			uint32_t i = 0;
			while (i < memProperties.memoryTypeCount)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return (i);
				++i;
			}
			exitOnError("Failed to find suitable memory type");
			return (0);
		}

		void createImage(const vk::Device &device, vk::PhysicalDeviceMemoryProperties memProperties, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image &image, vk::DeviceMemory &imageMemory)
		{
			vk::ImageCreateInfo imageInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, format, vk::Extent3D(width, height, 1), 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::ePreinitialized);
			image = device.createImage(imageInfo, CUSTOM_ALLOCATOR);
			// TODO: Write a sub allocator for image memory
			vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);
			vk::MemoryAllocateInfo allocInfo(memRequirements.size, getMemoryType(memProperties, memRequirements.memoryTypeBits, properties));
			imageMemory = device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
			LOG(INFO) << "image: " + std::to_string(memRequirements.size) + " " + std::to_string(memRequirements.memoryTypeBits) + " " + std::to_string(getMemoryType(memProperties, memRequirements.memoryTypeBits, properties));
			device.bindImageMemory(image, imageMemory, 0);
		}

		void copyImage(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::Image srcImage, vk::Image dstImage, uint32_t width, uint32_t height)
		{
			vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
			vk::ImageSubresourceLayers subResource(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
			vk::ImageCopy region(subResource, vk::Offset3D(0, 0, 0), subResource, vk::Offset3D(0, 0, 0), vk::Extent3D(width, height, 1));
			commandBuffer.copyImage(srcImage, vk::ImageLayout::eTransferSrcOptimal, dstImage, vk::ImageLayout::eTransferDstOptimal, 1, &region);
			endSingleTimeCommands(device, queue, commandPool, commandBuffer);
		}

		void createImageView(const vk::Device &device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView &imageView)
		{
			vk::ImageViewCreateInfo viewInfo(vk::ImageViewCreateFlags(), image, vk::ImageViewType::e2D, format);
			viewInfo.subresourceRange = vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1);
			imageView = device.createImageView(viewInfo, CUSTOM_ALLOCATOR);
		}

		void transitionImageLayout(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
		{
			vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
			vk::ImageMemoryBarrier barrier;
			barrier.setOldLayout(oldLayout);
			barrier.setNewLayout(newLayout);
			barrier.image = image;
			barrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
			if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
				barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
			if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferSrcOptimal)
			{
				barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
				barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
			}
			else if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
			{
				barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
				barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
			}
			else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			{
				barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
				barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
			}
			else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			{
				barrier.setSrcAccessMask(vk::AccessFlags());
				barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
			}
			else
				Tools::exitOnError("Unsupported layout transition");
			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
			endSingleTimeCommands(device, queue, commandPool, commandBuffer);
		}

		vk::CommandBuffer beginSingleTimeCommands(const vk::Device &device, const vk::CommandPool &commandPool)
		{
			vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
			std::vector<vk::CommandBuffer> commandBuffers = device.allocateCommandBuffers(allocInfo);
			vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			commandBuffers.at(0).begin(beginInfo);
			return (commandBuffers.at(0));
		}

		void endSingleTimeCommands(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::CommandBuffer commandBuffer)
		{
			vk::SubmitInfo submitInfo;

			commandBuffer.end();
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			queue.submit(submitInfo, VK_NULL_HANDLE);
			queue.waitIdle();
			device.freeCommandBuffers(commandPool, commandBuffer);
		}
	}
}
