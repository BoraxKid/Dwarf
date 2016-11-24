#include "Renderer.h"

namespace Dwarf
{
	Renderer::Renderer(int width, int height, const std::string &title)
		: _title(title), _mousePos(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f)
	{
        this->_numThreads = std::thread::hardware_concurrency();
        this->_threadPool.setThreadCount(this->_numThreads);
        this->_commandBufferBuilder = new CommandBufferBuilder(this->_device, this->_renderPass, this->_swapChainFramebuffers, this->_swapChainExtent, this->_threadPool, this->_numThreads);
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
        this->_materialManager = new MaterialManager(this->_device, this->_graphicsQueue, this->_renderPass, this->_swapChainExtent);
        this->_models.push_back(new Mesh(this->_device, *this->_materialManager, "models/lost_empire.obj"));
        //this->_models.push_back(new Mesh(this->_device, *this->_materialManager, "models/sphere.obj"));
        this->_materialManager->createDescriptorPool();

		std::vector<Mesh *>::iterator iterModels = this->_models.begin();
		std::vector<Mesh *>::iterator iterModels2 = this->_models.end();
		while (iterModels != iterModels2)
		{
            this->_commandBufferBuilder->addBuildables((*iterModels)->getBuildables());
			++iterModels;
		}
		this->createCommandBuffers();
		this->createSemaphores();
		this->run();
	}

	Renderer::~Renderer()
	{
		std::vector<Mesh *>::iterator iterModels = this->_models.begin();
		std::vector<Mesh *>::iterator iterModels2 = this->_models.end();
		while (iterModels != iterModels2)
		{
			delete (*iterModels);
			++iterModels;
		}
        delete (this->_commandBufferBuilder);
        delete (this->_materialManager);
		this->_device.destroySemaphore(this->_renderFinishedSemaphore, CUSTOM_ALLOCATOR);
		this->_device.destroySemaphore(this->_imageAvailableSemaphore, CUSTOM_ALLOCATOR);
		if (!this->_commandBuffers.empty())
			this->_device.freeCommandBuffers(this->_commandPool, this->_commandBuffers);
		this->_device.destroyImageView(this->_depthImageView, CUSTOM_ALLOCATOR);
		this->_device.freeMemory(this->_depthImageMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyImage(this->_depthImage, CUSTOM_ALLOCATOR);
		this->_device.destroyCommandPool(this->_commandPool, CUSTOM_ALLOCATOR);
		this->_device.destroyRenderPass(this->_renderPass, CUSTOM_ALLOCATOR);
		std::vector<vk::Framebuffer>::iterator iterFramebuffers = this->_swapChainFramebuffers.begin();
		std::vector<vk::Framebuffer>::iterator iterFramebuffers2 = this->_swapChainFramebuffers.end();
		while (iterFramebuffers != iterFramebuffers2)
		{
			this->_device.destroyFramebuffer(*iterFramebuffers, CUSTOM_ALLOCATOR);
			++iterFramebuffers;
		}
		std::vector<vk::ImageView>::iterator iterImageViews = this->_swapChainImageViews.begin();
		std::vector<vk::ImageView>::iterator iterImageViews2 = this->_swapChainImageViews.end();
		while (iterImageViews != iterImageViews2)
		{
			this->_device.destroyImageView(*iterImageViews, CUSTOM_ALLOCATOR);
			++iterImageViews;
		}
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
		if (this->_instance != (vk::Instance)VK_NULL_HANDLE)
			this->_instance.destroy(CUSTOM_ALLOCATOR);
		this->_instance = vk::createInstance(createInfo, CUSTOM_ALLOCATOR);

#ifdef _DEBUG
		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();
		std::vector<vk::ExtensionProperties>::const_iterator iter = extensions.begin();
		std::vector<vk::ExtensionProperties>::const_iterator iter2 = extensions.end();

		LOG(INFO) << "Available extensions:";
		while (iter != iter2)
		{
			LOG(INFO) << "\t" << iter->extensionName;
			++iter;
		}
#endif
	}

	void Renderer::createSurface()
	{
		if (this->_surface != (vk::SurfaceKHR)VK_NULL_HANDLE)
			this->_instance.destroySurfaceKHR(this->_surface, CUSTOM_ALLOCATOR);
		Tools::exitOnResult(static_cast<vk::Result>(glfwCreateWindowSurface((VkInstance)this->_instance, this->_window, CUSTOM_ALLOCATOR, (VkSurfaceKHR *)&this->_surface)));
	}

	void Renderer::pickPhysicalDevice()
	{
		std::vector<vk::PhysicalDevice> devices = this->_instance.enumeratePhysicalDevices();
		std::vector<vk::PhysicalDevice>::iterator iter = devices.begin();
		std::vector<vk::PhysicalDevice>::iterator iter2 = devices.end();

		while (iter != iter2)
		{
			if (this->isDeviceSuitable(*iter))
			{
				this->_physicalDevice = *iter;
				break;
			}
			++iter;
		}
		if (this->_physicalDevice == (vk::PhysicalDevice)VK_NULL_HANDLE)
			Tools::exitOnError("No suitable GPU");
	}

	void Renderer::createLogicalDevice()
	{
		Renderer::QueueFamilyIndices indices = this->findQueueFamilies(this->_physicalDevice);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
		std::set<uint32_t>::const_iterator iter = uniqueQueueFamilies.begin();
		std::set<uint32_t>::const_iterator iter2 = uniqueQueueFamilies.end();
		float queuePriority = 1.0f;

		while (iter != iter2)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), *iter, 1, &queuePriority);
			queueCreateInfos.push_back(queueCreateInfo);
			++iter;
		}
		vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.fillModeNonSolid = VK_TRUE;
		vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), 0, nullptr, static_cast<uint32_t>(gDeviceExtensions.size()), gDeviceExtensions.data(), &deviceFeatures);
		if (gEnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(gValidationLayers.size());
			createInfo.ppEnabledLayerNames = gValidationLayers.data();
		}
		if (this->_device != (vk::Device)VK_NULL_HANDLE)
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
		if (this->_swapChain != (vk::SwapchainKHR)VK_NULL_HANDLE)
			this->_device.destroySwapchainKHR(this->_swapChain, CUSTOM_ALLOCATOR);
		this->_swapChain = newSwapChain;
		this->_swapChainImages = this->_device.getSwapchainImagesKHR(this->_swapChain);
		this->_swapChainImageFormat = surfaceFormat.format;
		this->_swapChainExtent = extent;
		this->_camera.updateAspectRation(static_cast<float>(extent.width) / static_cast<float>(extent.height));
	}

	void Renderer::createImageViews()
	{
		std::vector<vk::ImageView>::iterator iter = this->_swapChainImageViews.begin();
		std::vector<vk::ImageView>::iterator iter2 = this->_swapChainImageViews.end();
		while (iter != iter2)
		{
			this->_device.destroyImageView(*iter, CUSTOM_ALLOCATOR);
			++iter;
		}
		this->_swapChainImageViews.resize(this->_swapChainImages.size());
		iter = this->_swapChainImageViews.begin();
		iter2 = this->_swapChainImageViews.end();
		while (iter != iter2)
		{
			Tools::createImageView(this->_device, this->_swapChainImages.at(iter - this->_swapChainImageViews.begin()), this->_swapChainImageFormat, vk::ImageAspectFlagBits::eColor, *iter);
			++iter;
		}
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
		if (this->_renderPass != (vk::RenderPass)VK_NULL_HANDLE)
			this->_device.destroyRenderPass(this->_renderPass, CUSTOM_ALLOCATOR);
		this->_renderPass = this->_device.createRenderPass(renderPassInfo, CUSTOM_ALLOCATOR);
	}

	void Renderer::createCommandPool()
	{
		Renderer::QueueFamilyIndices queueFamilyIndices = this->findQueueFamilies(this->_physicalDevice);
		vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamilyIndices.graphicsFamily);
		if (this->_commandPool != (vk::CommandPool)VK_NULL_HANDLE)
			this->_device.destroyCommandPool(this->_commandPool, CUSTOM_ALLOCATOR);
		this->_commandPool = this->_device.createCommandPool(poolInfo, CUSTOM_ALLOCATOR);
        this->_commandBufferBuilder->createCommandPools(queueFamilyIndices.graphicsFamily);
	}

	void Renderer::createDepthResources()
	{
		vk::Format depthFormat = this->findDepthFormat();
		if (this->_depthImageView != (vk::ImageView)VK_NULL_HANDLE)
			this->_device.destroyImageView(this->_depthImageView, CUSTOM_ALLOCATOR);
		if (this->_depthImageMemory != (vk::DeviceMemory)VK_NULL_HANDLE)
			this->_device.freeMemory(this->_depthImageMemory, CUSTOM_ALLOCATOR);
		if (this->_depthImage != (vk::Image)VK_NULL_HANDLE)
			this->_device.destroyImage(this->_depthImage, CUSTOM_ALLOCATOR);
		Tools::createImage(this->_device, this->_physicalDevice.getMemoryProperties(), this->_swapChainExtent.width, this->_swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, this->_depthImage, this->_depthImageMemory);
		Tools::createImageView(this->_device, this->_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, this->_depthImageView);
		Tools::transitionImageLayout(this->_device, this->_graphicsQueue, this->_commandPool, this->_depthImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	}

	void Renderer::createFramebuffers()
	{
		std::vector<vk::Framebuffer>::iterator iterFramebuffers = this->_swapChainFramebuffers.begin();
		std::vector<vk::Framebuffer>::iterator iterFramebuffers2 = this->_swapChainFramebuffers.end();
		while (iterFramebuffers != iterFramebuffers2)
		{
			this->_device.destroyFramebuffer(*iterFramebuffers, CUSTOM_ALLOCATOR);
			++iterFramebuffers;
		}
		this->_swapChainFramebuffers.resize(this->_swapChainImageViews.size());
		std::vector<vk::ImageView>::iterator iterImageViews = this->_swapChainImageViews.begin();
		std::vector<vk::ImageView>::iterator iterImageViews2 = this->_swapChainImageViews.end();
		std::array<vk::ImageView, 2> attachments;
		attachments.at(1) = this->_depthImageView;
		vk::FramebufferCreateInfo framebufferInfo(vk::FramebufferCreateFlags(), this->_renderPass, static_cast<uint32_t>(attachments.size()), nullptr, this->_swapChainExtent.width, this->_swapChainExtent.height, 1);
		size_t i;
		while (iterImageViews != iterImageViews2)
		{
			i = iterImageViews - this->_swapChainImageViews.begin();
			attachments.at(0) = *iterImageViews;
			framebufferInfo.setPAttachments(attachments.data());
			this->_swapChainFramebuffers.at(i) = this->_device.createFramebuffer(framebufferInfo, CUSTOM_ALLOCATOR);
			++iterImageViews;
		}
	}

	void Renderer::createCommandBuffers()
	{
		if (this->_commandBuffers.size() == this->_swapChainFramebuffers.size())
		{
			std::vector<vk::CommandBuffer>::iterator iter = this->_commandBuffers.begin();
			std::vector<vk::CommandBuffer>::iterator iter2 = this->_commandBuffers.end();

			while (iter != iter2)
			{
				iter->reset(vk::CommandBufferResetFlags());
				++iter;
			}
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
		/*vk::CommandBufferBeginInfo beginInfo;
		std::array<vk::ClearValue, 2> clearValues = { vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})), vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0)) };
		vk::RenderPassBeginInfo renderPassInfo(this->_renderPass, vk::Framebuffer(), vk::Rect2D(vk::Offset2D(0, 0), this->_swapChainExtent), static_cast<uint32_t>(clearValues.size()), clearValues.data());
		vk::CommandBufferInheritanceInfo inheritanceInfo(this->_renderPass);

		std::vector<Model *>::iterator iterModels = this->_models.begin();
		std::vector<Model *>::iterator iterModels2 = this->_models.end();
		std::vector<vk::CommandBuffer>::iterator iter = this->_commandBuffers.begin();
		std::vector<vk::CommandBuffer>::iterator iter2 = this->_commandBuffers.end();
		while (iter != iter2)
		{
			iterModels = this->_models.begin();
			iterModels2 = this->_models.end();
			renderPassInfo.framebuffer = this->_swapChainFramebuffers.at(iter - this->_commandBuffers.begin());
			inheritanceInfo.framebuffer = this->_swapChainFramebuffers.at(iter - this->_commandBuffers.begin());
			iter->begin(beginInfo);
			iter->beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
			while (iterModels != iterModels2)
			{
				(*iterModels)->buildCommandBuffers(inheritanceInfo, this->_camera.getMVP());
				iter->executeCommands((*iterModels)->getCommandBuffers());
				++iterModels;
			}
			iter->endRenderPass();
			iter->end();
			++iter;
		}*/
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

	bool Renderer::isDeviceSuitable(vk::PhysicalDevice device)
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

	Renderer::QueueFamilyIndices Renderer::findQueueFamilies(vk::PhysicalDevice device)
	{
		Renderer::QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
		std::vector<vk::QueueFamilyProperties>::const_iterator iter = queueFamilies.begin();
		std::vector<vk::QueueFamilyProperties>::const_iterator iter2 = queueFamilies.end();

		uint32_t i;
		vk::Bool32 presentSupport;
		while (iter != iter2)
		{
			i = static_cast<uint32_t>(iter - queueFamilies.begin());
			if (iter->queueCount > 0 && iter->queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilySet = true;
			}
			presentSupport = device.getSurfaceSupportKHR(i, this->_surface);
			if (iter->queueCount > 0 && presentSupport)
			{
				indices.presentFamily = i;
				indices.presentFamilySet = true;
			}
			if (indices.isComplete())
				break;
			++iter;
		}
		return (indices);
	}

	bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice device)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties(CUSTOM_ALLOCATOR);
		std::vector<vk::ExtensionProperties>::const_iterator iter = availableExtensions.begin();
		std::vector<vk::ExtensionProperties>::const_iterator iter2 = availableExtensions.end();
		std::set<std::string> requiredExtensions(gDeviceExtensions.begin(), gDeviceExtensions.end());

		while (iter != iter2)
		{
			requiredExtensions.erase(iter->extensionName);
			++iter;
		}
		return (requiredExtensions.empty());
	}

	Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(vk::PhysicalDevice device)
	{
		Renderer::SwapChainSupportDetails details;
		details.capabilities = device.getSurfaceCapabilitiesKHR(this->_surface);
		details.formats = device.getSurfaceFormatsKHR(this->_surface);
		details.presentModes = device.getSurfacePresentModesKHR(this->_surface);
		return (details);
	}

	vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
	{
		std::vector<vk::SurfaceFormatKHR>::const_iterator iter = availableFormats.begin();
		std::vector<vk::SurfaceFormatKHR>::const_iterator iter2 = availableFormats.end();

		if (availableFormats.size() == 1 && availableFormats.at(0).format == vk::Format::eUndefined)
		{
			vk::SurfaceFormatKHR tmp;
			tmp.format = vk::Format::eB8G8R8A8Unorm;
			tmp.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
			return (tmp);
		}
		while (iter != iter2)
		{
			if (iter->format == vk::Format::eB8G8R8A8Unorm && iter->colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return (*iter);
			++iter;
		}
		return (availableFormats.at(0));
	}

	vk::PresentModeKHR Renderer::chooseSwapPresentFormat(const std::vector<vk::PresentModeKHR> &availablePresentModes)
	{
		std::vector<vk::PresentModeKHR>::const_iterator iter = availablePresentModes.begin();
		std::vector<vk::PresentModeKHR>::const_iterator iter2 = availablePresentModes.end();

		while (iter != iter2)
		{
            if (*iter == vk::PresentModeKHR::eMailbox)
                return (*iter);
			++iter;
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

	vk::Format Renderer::findDepthFormat()
	{
		return (this->findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment));
	}

	vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
	{
		std::vector<vk::Format>::const_iterator iter = candidates.begin();
		std::vector<vk::Format>::const_iterator iter2 = candidates.end();
		vk::FormatProperties properties;

		while (iter != iter2)
		{
			properties = this->_physicalDevice.getFormatProperties(*iter);
			if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
				return (*iter);
			else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
				return (*iter);
			++iter;
		}
		Tools::exitOnError("Can't find a supported format");
		return (vk::Format());
	}

	uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		return (Tools::getMemoryType(this->_physicalDevice.getMemoryProperties(), typeFilter, properties));
	}

	void Renderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory)
	{
		vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), size, usage);
		buffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		vk::MemoryRequirements memRequirements = this->_device.getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, this->findMemoryType(memRequirements.memoryTypeBits, properties));
		bufferMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
		LOG(INFO) << "buffer: deviceSize " << size << " memReq.size " << memRequirements.size << " " << memRequirements.memoryTypeBits << " " << this->findMemoryType(memRequirements.memoryTypeBits, properties);
		this->_device.bindBufferMemory(buffer, bufferMemory, 0);
	}

	void Renderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
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

		if (action == GLFW_PRESS)
		{
            if (scancode == 32)
                camera->_left = true;
            else if (scancode == 30)
                camera->_right = true;
            else if (scancode == 17)
                camera->_up = true;
            else if (scancode == 31)
                camera->_down = true;
            else if (scancode == 42)
                camera->setCameraSpeed(10.0f);
		}
		else if (action == GLFW_RELEASE)
		{
			if (scancode == 32)
				camera->_left = false;
			else if (scancode == 30)
				camera->_right = false;
			else if (scancode == 17)
				camera->_up = false;
			else if (scancode == 31)
				camera->_down = false;
            else if (scancode == 42)
                camera->setCameraSpeed(1.0f);
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
