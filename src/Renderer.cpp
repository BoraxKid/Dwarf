#include "Renderer.h"

namespace Dwarf
{
	Renderer::Renderer(int width, int height, const std::string &title, bool fifo)
		: _title(title), _mousePos(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f), _fifo(fifo)
	{
        this->_numThreads = std::thread::hardware_concurrency();
        this->_threadPool.setThreadCount(this->_numThreads);
        this->_commandBufferBuilder = new CommandBuffersBuilder(this->_device, this->_renderPass, this->_swapChainFramebuffers, this->_swapChainExtent, this->_threadPool, this->_numThreads);
        this->createWindow(width, height, title);
		this->createInstance();
		this->setupDebugCallback();
		this->createSurface();
		this->pickPhysicalDevice();
		this->createLogicalDevice();
		this->createSwapChain();
		this->createImageViews();
		this->createRenderPass();
		this->createCommandPool();
		this->createDepthResources();
		this->createFramebuffers();
        this->_lightManager = new LightManager(this->_device, this->_physicalDevice.getMemoryProperties());
        this->_materialManager = new MaterialManager(this->_device, this->_graphicsQueue, this->_renderPass, this->_swapChainExtent);
        this->_models.push_back(new Mesh(this->_device, *this->_materialManager, "resources/models/CamaroSS.obj", this->_lightManager->getDescriptorBufferInfo()));
        this->_models.back()->setRotation(-90.0, 0.0, 0.0);
        this->_models.back()->setScale(5.0, 5.0, 5.0);
        this->_models.push_back(new Mesh(this->_device, *this->_materialManager, "resources/models/sphere.obj", this->_lightManager->getDescriptorBufferInfo()));
        this->_materialManager->createDescriptorPool();
        this->_deviceAllocator = new DeviceAllocationManager(this->_device, this->_graphicsQueue, this->_physicalDevice.getMemoryProperties());
        this->_deviceAllocator->allocate(this->_models, this->_commandPool);
        for (auto &model : this->_models)
            this->_commandBufferBuilder->addBuildables(model->getBuildables());
		this->createCommandBuffers();
		this->createSemaphores();
	}

	Renderer::~Renderer()
	{
        delete (this->_deviceAllocator);
		for (auto &model : this->_models)
            delete (model);
        delete (this->_commandBufferBuilder);
        delete (this->_materialManager);
        delete (this->_lightManager);
		this->_device.destroySemaphore(this->_renderFinishedSemaphore, CUSTOM_ALLOCATOR);
		this->_device.destroySemaphore(this->_imageAvailableSemaphore, CUSTOM_ALLOCATOR);
		if (!this->_commandBuffers.empty())
			this->_device.freeCommandBuffers(this->_commandPool, this->_commandBuffers);
		this->_device.destroyImageView(this->_depthImageView, CUSTOM_ALLOCATOR);
		this->_device.freeMemory(this->_depthImageMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyImage(this->_depthImage, CUSTOM_ALLOCATOR);
		this->_device.destroyCommandPool(this->_commandPool, CUSTOM_ALLOCATOR);
		this->_device.destroyRenderPass(this->_renderPass, CUSTOM_ALLOCATOR);
		for (const auto &framebuffer : this->_swapChainFramebuffers)
            this->_device.destroyFramebuffer(framebuffer, CUSTOM_ALLOCATOR);
        for (const auto &imageView : this->_swapChainImageViews)
            this->_device.destroyImageView(imageView, CUSTOM_ALLOCATOR);
		this->_device.destroySwapchainKHR(this->_swapChain, CUSTOM_ALLOCATOR);
		this->_device.destroy(CUSTOM_ALLOCATOR);
		this->_instance.destroySurfaceKHR(this->_surface, CUSTOM_ALLOCATOR);
		if (gEnableValidationLayers)
			Tools::DestroyDebugReportCallbackEXT(this->_instance, this->_callback, CUSTOM_ALLOCATOR);
		this->_instance.destroy(CUSTOM_ALLOCATOR);
	}

	void Renderer::run()
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		float diff;
		uint32_t frameCounter = 0;
		float frameTimer = 0.0f;
		float fpsTimer = 0.0f;
		uint32_t lastFps = 0;
		std::string tmp;
		tmp = this->_title + " - " + std::to_string(frameCounter) + " fps (" + std::to_string(frameTimer) + " ms)";
		glfwSetWindowTitle(this->_window, tmp.c_str());
		while (!glfwWindowShouldClose(this->_window))
		{
			glfwPollEvents();
			start = std::chrono::high_resolution_clock::now();
			this->_camera.update(frameTimer);
            this->_lightManager->updateLightPos(frameTimer);
            if (this->_movance.down)
                this->_models.at(0)->move(0.0, 0.0, -10 * frameTimer);
            if (this->_movance.up)
                this->_models.at(0)->move(0.0, 0.0, 10 * frameTimer);
            if (this->_movance.left)
                this->_models.at(0)->move(-10 * frameTimer, 0.0, 0.0);
            if (this->_movance.right)
                this->_models.at(0)->move(10 * frameTimer, 0.0, 0.0);
			this->buildCommandBuffers();
			this->drawFrame();
			++frameCounter;
			end = std::chrono::high_resolution_clock::now();
			diff = std::chrono::duration<float, std::milli>(end - start).count();
			frameTimer = diff / 1000.0f;
			fpsTimer += diff;
			if (fpsTimer > 1000.0f)
			{
				lastFps = frameCounter;
				tmp = this->_title + " - " + std::to_string(frameCounter) + " fps (" + std::to_string(frameTimer) + " ms)";
				glfwSetWindowTitle(this->_window, tmp.c_str());
				fpsTimer = 0.0f;
				frameCounter = 0;
			}
		}
		this->_device.waitIdle();
	}

	void Renderer::createWindow(int width, int height, const std::string &title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		this->_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		glfwSetInputMode(this->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetWindowPos(this->_window, -1920, 100);
		glfwSetWindowUserPointer(this->_window, this);
		glfwSetWindowSizeCallback(this->_window, Renderer::onWindowResized);

		glfwSetCursorPosCallback(this->_window, Renderer::onCursorMovement);
		glfwSetKeyCallback(this->_window, Renderer::onKeyInput);

		// The following part is done so the cursor move event from the input mode is not processed in the main loop
		glfwPollEvents();
		double tmp[2];
		glfwGetCursorPos(this->_window, &tmp[0], &tmp[1]);
		this->_mousePos = glm::vec2(static_cast<float>(tmp[0]), static_cast<float>(tmp[1]));
		this->_camera.setRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		// TODO: check return values
	}

	void Renderer::createInstance()
	{
		if (gEnableValidationLayers && !Tools::checkValidationLayerSupport(gValidationLayers))
			Tools::exitOnError("Validation layers not available");
		vk::ApplicationInfo appInfo(this->_title.c_str(), VK_MAKE_VERSION(1, 0, 0), "Dwarf Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
		std::vector<const char *> requiredExtensions = Tools::getRequiredExtensions(gEnableValidationLayers);
		vk::InstanceCreateInfo createInfo;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		if (gEnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(gValidationLayers.size());
			createInfo.ppEnabledLayerNames = gValidationLayers.data();
		}
		if (this->_instance)
			this->_instance.destroy(CUSTOM_ALLOCATOR);
		this->_instance = vk::createInstance(createInfo, CUSTOM_ALLOCATOR);

#ifdef _DEBUG
		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

		LOG(INFO) << "Available extensions:";
		for (const auto &extension : extensions)
			LOG(INFO) << "\t" << extension.extensionName;
#endif
	}

	void Renderer::createSurface()
	{
		if (this->_surface)
			this->_instance.destroySurfaceKHR(this->_surface, CUSTOM_ALLOCATOR);
		Tools::exitOnResult(static_cast<vk::Result>(glfwCreateWindowSurface((VkInstance)this->_instance, this->_window, CUSTOM_ALLOCATOR, (VkSurfaceKHR *)&this->_surface)));
	}

	void Renderer::pickPhysicalDevice()
	{
		std::vector<vk::PhysicalDevice> physicalDevices = this->_instance.enumeratePhysicalDevices();
		
        for (const auto &physicalDevice : physicalDevices)
        {
            if (this->isDeviceSuitable(physicalDevice))
            {
                this->_physicalDevice = physicalDevice;
                break;
            }
        }
		if (!this->_physicalDevice)
			Tools::exitOnError("No suitable GPU");
	}

	void Renderer::createLogicalDevice()
	{
		Renderer::QueueFamilyIndices indices = this->findQueueFamilies(this->_physicalDevice);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
		float queuePriority = 1.0f;

        vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), 0, 1, &queuePriority);
        for (const auto &uniqueQueueFamily : uniqueQueueFamilies)
        {
            queueCreateInfo.queueFamilyIndex = uniqueQueueFamily;
            queueCreateInfos.push_back(queueCreateInfo);
        }
		vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.fillModeNonSolid = VK_TRUE;
		vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), 0, nullptr, static_cast<uint32_t>(gDeviceExtensions.size()), gDeviceExtensions.data(), &deviceFeatures);
		if (gEnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(gValidationLayers.size());
			createInfo.ppEnabledLayerNames = gValidationLayers.data();
		}
		if (this->_device)
			this->_device.destroy(CUSTOM_ALLOCATOR);
		this->_device = this->_physicalDevice.createDevice(createInfo, CUSTOM_ALLOCATOR);
		this->_graphicsQueue = this->_device.getQueue(indices.graphicsFamily, 0);
		this->_presentQueue = this->_device.getQueue(indices.presentFamily, 0);
	}

	void Renderer::createSwapChain()
	{
		Renderer::SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(this->_physicalDevice);
		vk::SurfaceFormatKHR surfaceFormat = this->chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = this->chooseSwapPresentFormat(swapChainSupport.presentModes);
		vk::Extent2D extent = this->chooseSwapExtent(swapChainSupport.capabilities);
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;
		vk::SwapchainKHR oldSwapChain = this->_swapChain;
		vk::SwapchainCreateInfoKHR createInfo(vk::SwapchainCreateFlagsKHR(), this->_surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, swapChainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, VK_TRUE, oldSwapChain);
		Renderer::QueueFamilyIndices indices = this->findQueueFamilies(this->_physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		vk::SwapchainKHR newSwapChain = this->_device.createSwapchainKHR(createInfo, CUSTOM_ALLOCATOR);
		if (this->_swapChain)
			this->_device.destroySwapchainKHR(this->_swapChain, CUSTOM_ALLOCATOR);
		this->_swapChain = newSwapChain;
		this->_swapChainImages = this->_device.getSwapchainImagesKHR(this->_swapChain);
		this->_swapChainImageFormat = surfaceFormat.format;
		this->_swapChainExtent = extent;
		this->_camera.updateAspectRation(static_cast<float>(extent.width) / static_cast<float>(extent.height));
	}

	void Renderer::createImageViews()
	{
        for (const auto &imageView : this->_swapChainImageViews)
            this->_device.destroyImageView(imageView, CUSTOM_ALLOCATOR);
		this->_swapChainImageViews.resize(this->_swapChainImages.size());
        size_t i = 0;
        for (auto &imageView : this->_swapChainImageViews)
            Tools::createImageView(this->_device, this->_swapChainImages.at(i++), this->_swapChainImageFormat, vk::ImageAspectFlagBits::eColor, imageView);
	}

	void Renderer::createRenderPass()
	{
		vk::AttachmentDescription colorAttachment(vk::AttachmentDescriptionFlags(), this->_swapChainImageFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		vk::AttachmentDescription depthAttachment(vk::AttachmentDescriptionFlags(), this->findDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal);
		vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
		vk::SubpassDescription subPass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef, nullptr, &depthAttachmentRef);
		vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
		std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		vk::RenderPassCreateInfo renderPassInfo(vk::RenderPassCreateFlags(), static_cast<uint32_t>(attachments.size()), attachments.data(), 1, &subPass, 1, &dependency);
		if (this->_renderPass)
			this->_device.destroyRenderPass(this->_renderPass, CUSTOM_ALLOCATOR);
		this->_renderPass = this->_device.createRenderPass(renderPassInfo, CUSTOM_ALLOCATOR);
	}

	void Renderer::createCommandPool()
	{
		Renderer::QueueFamilyIndices queueFamilyIndices = this->findQueueFamilies(this->_physicalDevice);
		vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndices.graphicsFamily);
		if (this->_commandPool)
			this->_device.destroyCommandPool(this->_commandPool, CUSTOM_ALLOCATOR);
		this->_commandPool = this->_device.createCommandPool(poolInfo, CUSTOM_ALLOCATOR);
        this->_commandBufferBuilder->createCommandPools(queueFamilyIndices.graphicsFamily);
	}

	void Renderer::createDepthResources()
	{
		vk::Format depthFormat = this->findDepthFormat();
		if (this->_depthImageView)
			this->_device.destroyImageView(this->_depthImageView, CUSTOM_ALLOCATOR);
		if (this->_depthImageMemory)
			this->_device.freeMemory(this->_depthImageMemory, CUSTOM_ALLOCATOR);
		if (this->_depthImage)
			this->_device.destroyImage(this->_depthImage, CUSTOM_ALLOCATOR);
		Tools::createImage(this->_device, this->_physicalDevice.getMemoryProperties(), this->_swapChainExtent.width, this->_swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, this->_depthImage, this->_depthImageMemory);
		Tools::createImageView(this->_device, this->_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, this->_depthImageView);
		Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPool, this->_depthImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	}

	void Renderer::createFramebuffers()
	{
        for (const auto &framebuffer : this->_swapChainFramebuffers)
            this->_device.destroyFramebuffer(framebuffer, CUSTOM_ALLOCATOR);
		this->_swapChainFramebuffers.resize(this->_swapChainImageViews.size());
		std::array<vk::ImageView, 2> attachments;
		attachments.at(1) = this->_depthImageView;
		vk::FramebufferCreateInfo framebufferInfo(vk::FramebufferCreateFlags(), this->_renderPass, static_cast<uint32_t>(attachments.size()), nullptr, this->_swapChainExtent.width, this->_swapChainExtent.height, 1);
		size_t i = 0;
        for (const auto &imageView : this->_swapChainImageViews)
		{
			attachments.at(0) = imageView;
			framebufferInfo.setPAttachments(attachments.data());
			this->_swapChainFramebuffers.at(i) = this->_device.createFramebuffer(framebufferInfo, CUSTOM_ALLOCATOR);
            ++i;
		}
	}

	void Renderer::createCommandBuffers()
	{
		if (this->_commandBuffers.size() == this->_swapChainFramebuffers.size())
		{
            for (const auto &commandBuffer : this->_commandBuffers)
                commandBuffer.reset(vk::CommandBufferResetFlags(vk::CommandBufferResetFlagBits::eReleaseResources));
		}
		else
		{
			if (!this->_commandBuffers.empty())
				this->_device.freeCommandBuffers(this->_commandPool, this->_commandBuffers);
			this->_commandBuffers.resize(this->_swapChainFramebuffers.size());
			vk::CommandBufferAllocateInfo allocInfo(this->_commandPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(this->_commandBuffers.size()));
			this->_commandBuffers = this->_device.allocateCommandBuffers(allocInfo);
		}
        this->_commandBufferBuilder->createCommandBuffers(this->_graphicsQueue, this->_physicalDevice.getMemoryProperties());
		this->buildCommandBuffers();
	}

	void Renderer::buildCommandBuffers()
	{
        this->_commandBufferBuilder->buildCommandBuffers(this->_commandBuffers, this->_camera.getMVP());
	}

	void Renderer::createSemaphores()
	{
		vk::SemaphoreCreateInfo semaphoreInfo;
		this->_imageAvailableSemaphore = this->_device.createSemaphore(semaphoreInfo, CUSTOM_ALLOCATOR);
		this->_renderFinishedSemaphore = this->_device.createSemaphore(semaphoreInfo, CUSTOM_ALLOCATOR);
	}

	void Renderer::drawFrame()
	{
        vk::ResultValue<uint32_t> imageIndex = this->_device.acquireNextImageKHR(this->_swapChain, std::numeric_limits<uint64_t>::max(), this->_imageAvailableSemaphore, VK_NULL_HANDLE);
		if (imageIndex.result == vk::Result::eErrorOutOfDateKHR)
		{
			this->recreateSwapChain();
			return;
		}
		else if (imageIndex.result != vk::Result::eSuccess && imageIndex.result != vk::Result::eSuboptimalKHR)
			Tools::exitOnResult(imageIndex.result);
		vk::PipelineStageFlags waitStages(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		vk::SubmitInfo submitInfo(1, &this->_imageAvailableSemaphore, &waitStages, 1, &this->_commandBuffers.at(imageIndex.value), 1, &this->_renderFinishedSemaphore);
		this->_graphicsQueue.submit(submitInfo, nullptr);
		vk::PresentInfoKHR presentInfo(1, &this->_renderFinishedSemaphore, 1, &this->_swapChain, &imageIndex.value, nullptr);
		vk::Result result = this->_presentQueue.presentKHR(presentInfo);
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
			this->recreateSwapChain();
		else if (result != vk::Result::eSuccess)
			Tools::exitOnResult(result);
		this->_presentQueue.waitIdle();
	}

	void Renderer::setupDebugCallback()
	{
		if (!gEnableValidationLayers)
			return;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)this->debugCallback;

		PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(this->_instance, "vkCreateDebugReportCallbackEXT");

		if (!CreateDebugReportCallback || CreateDebugReportCallback(this->_instance, &createInfo, nullptr, &this->_callback) != VK_SUCCESS)
			throw (std::runtime_error("Failed to set up debug callback"));
	}

	bool Renderer::isDeviceSuitable(const vk::PhysicalDevice &device) const
	{
		Renderer::QueueFamilyIndices indices = this->findQueueFamilies(device);
		bool extensionsSupported = this->checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;

		if (extensionsSupported)
		{
			Renderer::SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return (indices.isComplete() && extensionsSupported && swapChainAdequate);
	}

	Renderer::QueueFamilyIndices Renderer::findQueueFamilies(const vk::PhysicalDevice &device) const
	{
		Renderer::QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		uint32_t i = 0;
		vk::Bool32 presentSupport;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
                indices.graphicsFamilySet = true;
            }
            presentSupport = device.getSurfaceSupportKHR(i, this->_surface);
            if (queueFamily.queueCount > 0 && presentSupport)
            {
                indices.presentFamily = i;
                indices.presentFamilySet = true;
            }
            if (indices.isComplete())
                break;
            ++i;
        }
		return (indices);
	}

	bool Renderer::checkDeviceExtensionSupport(const vk::PhysicalDevice &device) const
	{
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties(CUSTOM_ALLOCATOR);
		std::set<std::string> requiredExtensions(gDeviceExtensions.begin(), gDeviceExtensions.end());

        for (const auto &extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);
		return (requiredExtensions.empty());
	}

	Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(const vk::PhysicalDevice &device) const
	{
		Renderer::SwapChainSupportDetails details;
		details.capabilities = device.getSurfaceCapabilitiesKHR(this->_surface);
		details.formats = device.getSurfaceFormatsKHR(this->_surface);
		details.presentModes = device.getSurfacePresentModesKHR(this->_surface);
		return (details);
	}

	vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
	{
		if (availableFormats.size() == 1 && availableFormats.at(0).format == vk::Format::eUndefined)
		{
			vk::SurfaceFormatKHR tmp;
			tmp.format = vk::Format::eB8G8R8A8Unorm;
			tmp.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
			return (tmp);
		}
		for (const auto &format : availableFormats)
		{
			if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return (format);
		}
		return (availableFormats.at(0));
	}

    vk::PresentModeKHR Renderer::chooseSwapPresentFormat(const std::vector<vk::PresentModeKHR> &availablePresentModes)
    {
        if (!this->_fifo)
            for (const auto &presentMode : availablePresentModes)
            {
                if (presentMode == vk::PresentModeKHR::eMailbox)
                    return (presentMode);
            }
        return (vk::PresentModeKHR::eFifo);
    }

	vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return (capabilities.currentExtent);
		else
		{
			vk::Extent2D actualExtent(1280, 720);
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return (actualExtent);
		}
	}

	vk::Format Renderer::findDepthFormat() const
	{
		return (this->findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment));
	}

	vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
	{
		vk::FormatProperties properties;

		for (const auto &candidate : candidates)
		{
			properties = this->_physicalDevice.getFormatProperties(candidate);
			if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
				return (candidate);
			else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
				return (candidate);
		}
		Tools::exitOnError("Can't find a supported format");
		return (vk::Format());
	}

	uint32_t Renderer::findMemoryType(uint32_t typeFilter, const vk::MemoryPropertyFlags &properties) const
	{
		return (Tools::getMemoryType(this->_physicalDevice.getMemoryProperties(), typeFilter, properties));
	}

	void Renderer::createBuffer(const vk::DeviceSize &size, const vk::BufferUsageFlags &usage, const vk::MemoryPropertyFlags &properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory) const
	{
		vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), size, usage);
		buffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		vk::MemoryRequirements memRequirements = this->_device.getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, this->findMemoryType(memRequirements.memoryTypeBits, properties));
		bufferMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
		LOG(INFO) << "buffer: deviceSize " << size << " memReq.size " << memRequirements.size << " " << memRequirements.memoryTypeBits << " " << this->findMemoryType(memRequirements.memoryTypeBits, properties);
		this->_device.bindBufferMemory(buffer, bufferMemory, 0);
	}

	void Renderer::copyBuffer(const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size) const
	{
		vk::CommandBuffer commandBuffer = Tools::beginSingleTimeCommands(this->_device, this->_commandPool);
		vk::BufferCopy copyRegion(0, 0, size);
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
		Tools::endSingleTimeCommands(this->_device, this->_graphicsQueue, this->_commandPool, commandBuffer);
	}

	void Renderer::recreateSwapChain()
	{
		this->_device.waitIdle();
		this->createSwapChain();
		this->createImageViews();
		this->createRenderPass();
        this->_materialManager->recreatePipelines();
		this->createDepthResources();
		this->createFramebuffers();
		this->createCommandBuffers();
	}

	void Renderer::onWindowResized(GLFWwindow *window, int width, int height)
	{
		if (width == 0 || height == 0)
			return;
		Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
		renderer->recreateSwapChain();
	}

	void Renderer::onCursorMovement(GLFWwindow *window, double x, double y)
	{
		glm::vec2 cursor(x, y);
		Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
		renderer->getCamera()->rotate(glm::vec3((renderer->_mousePos.y - cursor.y), (renderer->_mousePos.x - cursor.x), 0.0f));
		renderer->_mousePos = cursor;
	}

	void Renderer::onKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		Renderer *renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
		Camera *camera = renderer->getCamera();

        std::cout << scancode << std::endl;
		if (action == GLFW_PRESS)
		{
            if (scancode == 32)
                camera->_right = true;
            else if (scancode == 30)
                camera->_left = true;
            else if (scancode == 17)
                camera->_up = true;
            else if (scancode == 31)
                camera->_down = true;
            else if (scancode == 42)
                camera->setCameraSpeed(10.0f);
            else if (scancode == 328)
                renderer->_movance.up = true;
            else if (scancode == 336)
                renderer->_movance.down = true;
            else if (scancode == 331)
                renderer->_movance.left = true;
            else if (scancode == 333)
                renderer->_movance.right = true;
            else if (scancode == 56) // ALT
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (action == GLFW_RELEASE)
		{
			if (scancode == 32)
				camera->_right = false;
			else if (scancode == 30)
				camera->_left = false;
			else if (scancode == 17)
				camera->_up = false;
			else if (scancode == 31)
				camera->_down = false;
            else if (scancode == 42)
                camera->setCameraSpeed(1.0f);
            else if (scancode == 328)
                renderer->_movance.up = false;
            else if (scancode == 336)
                renderer->_movance.down = false;
            else if (scancode == 331)
                renderer->_movance.left = false;
            else if (scancode == 333)
                renderer->_movance.right = false;
            else if (scancode == 56) // ALT
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	vk::Bool32 Renderer::debugCallback(vk::DebugReportFlagsEXT flags, vk::DebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layerPrefix, const char *msg, void *userData)
	{
		LOG(WARNING) << "Validation layer: " << msg;
		return (VK_FALSE);
	}
	
	Camera *Renderer::getCamera()
	{
		return (&this->_camera);
	}
}
