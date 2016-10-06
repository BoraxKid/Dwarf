#ifndef TOOLS_H_
#define TOOLS_H_
#pragma once

#include <vector>
#include <easylogging++.h>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define CUSTOM_ALLOCATOR nullptr

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

namespace Dwarf
{
	namespace Tools
	{
		bool checkValidationLayerSupport(const std::vector<const char *> validationLayers);
		std::vector<const char *> getRequiredExtensions(bool validationLayerEnabled);
		VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback);
		void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks *pAllocator);
		uint32_t getMemoryType(vk::PhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		void createImage(const vk::Device &device, vk::PhysicalDeviceMemoryProperties memProperties, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image &image, vk::DeviceMemory &imageMemory);
		void copyImage(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::Image srcImage, vk::Image dstImage, uint32_t width, uint32_t height);
		void createImageView(const vk::Device &device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView &imageView);
		void transitionImageLayout(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		vk::CommandBuffer beginSingleTimeCommands(const vk::Device &device, const vk::CommandPool &commandPool);
		void endSingleTimeCommands(const vk::Device &device, const vk::Queue &queue, const vk::CommandPool &commandPool, vk::CommandBuffer commandBuffer);
		
		inline void exitOnError(const std::string &error, const char *file, const int line)
		{
			std::string tmp = "In file " + std::string(file) + " at line " + std::to_string(line) + ": " + error;
			LOG(FATAL) << tmp;
			throw (std::runtime_error(tmp));
		}

		inline void exitOnResult(const vk::Result &result, const char *file, const int line)
		{
			if (result != vk::Result::eSuccess)
			{
				std::string tmp = "In file " + std::string(file) + " at line " + std::to_string(line) + ": " + vk::to_string(result);
				LOG(FATAL) << tmp;
				throw (std::runtime_error(tmp));
			}
		}

#define exitOnError(error) exitOnError(error, __FILENAME__, __LINE__)
#define exitOnResult(result) exitOnResult(result, __FILENAME__, __LINE__)

		static std::vector<char> readFile(const std::string &filename)
		{
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!file.is_open())
				exitOnError("Failed to open file " + filename);
			size_t fileSize = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();
			return (buffer);
		}
	}
}

#endif // TOOLS_H_
