#ifndef RENDERER_H_
#define RENDERER_H_
#pragma once

#include <chrono>
#include <set>
#include <unordered_map>
#include <easylogging++.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Tools.h"
#include "Mesh.h"
#include "Camera.h"
#include "CommandBufferBuilder.h"

const std::vector<const char *> gValidationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char *> gDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef _DEBUG
const bool gEnableValidationLayers = true;
#else
const bool gEnableValidationLayers = false;
#endif

/// \namespace Dwarf
/// \brief Entire engine's namespace
///
/// Namespace used throughout the entire engine
namespace Dwarf
{
	/// \class Renderer
	/// \brief Rendering class
	///
	/// Class handling GLFW window and everything related to Vulkan
	/// (Instance, Surface, Devices, Queues, Swapchain, Renderpass, Commandpool ...)
	class Renderer
	{
	public:
		/// \struct QueueFamilyIndices
		/// \brief Used to keep indices of both graphics and present queues
		struct QueueFamilyIndices
		{
			QueueFamilyIndices() : graphicsFamilySet(false), presentFamilySet(false) {}
			bool isComplete() { return (this->graphicsFamilySet && this->presentFamilySet); }

			uint32_t graphicsFamily;
			bool graphicsFamilySet;
			uint32_t presentFamily;
			bool presentFamilySet;
		};

		/// \struct SwapChainSupportDetails
		/// \brief Used to keep details about the capabilities of the surface
		struct SwapChainSupportDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		Renderer(int width = 1280, int height = 720, const std::string &title = "Vulkan Renderer");
		virtual ~Renderer();

		/// \brief Update and displaye on screen every frame
		void run();

	private:
		/// \brief Create the GLFW window
		/// \todo Check the return values of GLFW functions
		void createWindow(int width, int height, const std::string &title);
		/// \brief Check Layers and required extensions and create the Vulkan Instance
		void createInstance();
		/// \brief Create the surface with GLFW
		void createSurface();
		/// \brief Pick a physical device
		/// \todo Pick the best physical device instead of the first suitable one
		void pickPhysicalDevice();
		/// \brief Create a device and get graphics and present queues indices
		void createLogicalDevice();
		/// \brief Create the swapchain and get swapchain images, format and extent
		void createSwapChain();
		/// \brief Create swap chain image views
		void createImageViews();
		/// \brief Create the render pass
		void createRenderPass();
		/// \brief Create the command pool
		void createCommandPool();
		/// \brief Create the depth image
		void createDepthResources();
		/// \brief Create frame buffers
		void createFramebuffers();
		/// \brief Create the command buffers
		void createCommandBuffers();
		/// \brief Build and execute command buffers
		void buildCommandBuffers();
		/// \brief Create semaphores
		void createSemaphores();
		/// \brief Submit the primary command buffers to the graphics queue and present
		void drawFrame();

		void setupDebugCallback();
        bool isDeviceSuitable(const vk::PhysicalDevice &device) const;
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device) const;
        bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device) const;
        SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device) const;
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
		vk::PresentModeKHR chooseSwapPresentFormat(const std::vector<vk::PresentModeKHR> &availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
		vk::Format findDepthFormat() const;
		vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
		uint32_t findMemoryType(uint32_t typeFilter, const vk::MemoryPropertyFlags &properties) const;
		
		void createBuffer(const vk::DeviceSize &size, const vk::BufferUsageFlags &usage, const vk::MemoryPropertyFlags &properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory) const;
		void copyBuffer(const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size) const;
		void recreateSwapChain();

		static void onWindowResized(GLFWwindow *window, int width, int height);
		static void onCursorMovement(GLFWwindow *window, double x, double y);
		static void onKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods);
		static vk::Bool32 debugCallback(vk::DebugReportFlagsEXT flags, vk::DebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layerPrefix, const char *msg, void *userData);

		Camera *getCamera();
        Mesh *getModel() { return (this->_models.at(0)); }

		const std::string &_title;
		GLFWwindow *_window;
		vk::Instance _instance;
		VkDebugReportCallbackEXT _callback;
		vk::SurfaceKHR _surface;

		vk::PhysicalDevice _physicalDevice;
		vk::Device _device;

		vk::Queue _graphicsQueue;
		vk::Queue _presentQueue;

		vk::DeviceMemory _imageMemory;
		vk::DeviceMemory _bufferMemory;

		vk::SwapchainKHR _swapChain;
		std::vector<vk::Image> _swapChainImages;
		vk::Format _swapChainImageFormat;
		vk::Extent2D _swapChainExtent;
		std::vector<vk::ImageView> _swapChainImageViews;
		std::vector<vk::Framebuffer> _swapChainFramebuffers;

		vk::RenderPass _renderPass;

		vk::CommandPool _commandPool;

		vk::Image _depthImage;
		vk::DeviceMemory _depthImageMemory;
		vk::ImageView _depthImageView;

		std::vector<vk::CommandBuffer> _commandBuffers;

		vk::Semaphore _imageAvailableSemaphore;
		vk::Semaphore _renderFinishedSemaphore;

		Camera _camera;
		glm::vec2 _mousePos;

        MaterialManager *_materialManager;
		std::vector<Mesh *> _models;
        ThreadPool _threadPool;
        CommandBufferBuilder *_commandBufferBuilder;
        uint32_t _numThreads;
	};
}

#endif // RENDERER_H_
