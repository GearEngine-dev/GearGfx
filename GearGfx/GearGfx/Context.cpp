#include<Context.h>
#include <algorithm>
GFX_NAMESPACE_BEGIN
Context::Context()
{
	//初始化扩展列表
	mValidationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	mDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

Context::~Context()
{
	vkDestroyInstance(mInstance, nullptr);
}

bool Context::initial()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Gear";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Gear";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
	mInitial = true;
	return true;
}

bool Context::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : mValidationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}

std::vector<const char*> Context::getRequiredExtensions()
{
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++)
	{
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


Device * Context::createDevice(VkSurfaceKHR surface)
{
	if (!mInitial)
	{
		throw std::runtime_error("must to create instance!");
		return nullptr;
	}
	
	uint32_t mGpuCount = 0;
	if (vkEnumeratePhysicalDevices(mInstance, &mGpuCount, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to find mGpu!");
		return nullptr;
	}

	if (mGpuCount == 0)
	{
		throw std::runtime_error("failed to find mGpu!");
		return nullptr;
	}

	std::vector<VkPhysicalDevice> gpus(mGpuCount);
	if (vkEnumeratePhysicalDevices(mInstance, &mGpuCount, gpus.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to find mGpu!");
		return nullptr;
	}

	for (auto &g : gpus)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(g, &props);
		LOGI("Found Vulkan GPU: %s\n", props.deviceName);
		LOGI("    API: %u.%u.%u\n",
			VK_VERSION_MAJOR(props.apiVersion),
			VK_VERSION_MINOR(props.apiVersion),
			VK_VERSION_PATCH(props.apiVersion));
		LOGI("    Driver: %u.%u.%u\n",
			VK_VERSION_MAJOR(props.driverVersion),
			VK_VERSION_MINOR(props.driverVersion),
			VK_VERSION_PATCH(props.driverVersion));
	}
	//默认选择第一块显卡
	mGpu = gpus.front();

	uint32_t graphicsQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	uint32_t computeQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	uint32_t transferQueueFamily = VK_QUEUE_FAMILY_IGNORED;

	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &queueCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueProps(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &queueCount, queueProps.data());

	for (unsigned i = 0; i < queueCount; i++)
	{
		VkBool32 supported = surface == VK_NULL_HANDLE;
		if (surface != VK_NULL_HANDLE)
			vkGetPhysicalDeviceSurfaceSupportKHR(mGpu, i, surface, &supported);

		static const VkQueueFlags required = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT;
		if (supported && ((queueProps[i].queueFlags & required) == required))
		{
			graphicsQueueFamily = i;
			break;
		}
	}

	for (unsigned i = 0; i < queueCount; i++)
	{
		static const VkQueueFlags required = VK_QUEUE_COMPUTE_BIT;
		if (i != graphicsQueueFamily && (queueProps[i].queueFlags & required) == required)
		{
			computeQueueFamily = i;
			break;
		}
	}

	for (unsigned i = 0; i < queueCount; i++)
	{
		static const VkQueueFlags required = VK_QUEUE_TRANSFER_BIT;
		if (i != graphicsQueueFamily && i != computeQueueFamily && (queueProps[i].queueFlags & required) == required)
		{
			transferQueueFamily = i;
			break;
		}
	}

	if (transferQueueFamily == VK_QUEUE_FAMILY_IGNORED)
	{
		for (unsigned i = 0; i < queueCount; i++)
		{
			static const VkQueueFlags required = VK_QUEUE_TRANSFER_BIT;
			if (i != graphicsQueueFamily && (queueProps[i].queueFlags & required) == required)
			{
				transferQueueFamily = i;
				break;
			}
		}
	}

	if (graphicsQueueFamily == VK_QUEUE_FAMILY_IGNORED)
		return nullptr;

	uint32_t universalQueueIndex = 1;
	uint32_t graphicsQueueIndex = 0;
	uint32_t computeQueueIndex = 0;
	uint32_t transferQueueIndex = 0;

	if (computeQueueFamily == VK_QUEUE_FAMILY_IGNORED)
	{
		computeQueueFamily = graphicsQueueFamily;
		computeQueueIndex = std::min(queueProps[graphicsQueueFamily].queueCount - 1, universalQueueIndex);
		universalQueueIndex++;
	}

	if (transferQueueFamily == VK_QUEUE_FAMILY_IGNORED)
	{
		transferQueueFamily = graphicsQueueFamily;
		transferQueueIndex = std::min(queueProps[graphicsQueueFamily].queueCount - 1, universalQueueIndex);
		universalQueueIndex++;
	}
	else if (transferQueueFamily == computeQueueFamily)
		transferQueueIndex = std::min(queueProps[computeQueueFamily].queueCount - 1, 1u);

	static const float graphicsQueuePrio = 0.5f;
	static const float computeQueuePrio = 1.0f;
	static const float transferQueuePrio = 1.0f;
	float prio[3] = { graphicsQueuePrio, computeQueuePrio, transferQueuePrio };

	unsigned queueFamilyCount = 0;
	VkDeviceQueueCreateInfo queueInfo[3] = {};

	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.pQueueCreateInfos = queueInfo;

	queueInfo[queueFamilyCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo[queueFamilyCount].queueFamilyIndex = graphicsQueueFamily;
	queueInfo[queueFamilyCount].queueCount = std::min(universalQueueIndex,queueProps[graphicsQueueFamily].queueCount);
	queueInfo[queueFamilyCount].pQueuePriorities = prio;
	queueFamilyCount++;

	if (computeQueueFamily != graphicsQueueFamily)
	{
		queueInfo[queueFamilyCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo[queueFamilyCount].queueFamilyIndex = computeQueueFamily;
		queueInfo[queueFamilyCount].queueCount = std::min(transferQueueFamily == computeQueueFamily ? 2u : 1u,queueProps[computeQueueFamily].queueCount);
		queueInfo[queueFamilyCount].pQueuePriorities = prio + 1;
		queueFamilyCount++;
	}

	if (transferQueueFamily != graphicsQueueFamily && transferQueueFamily != computeQueueFamily)
	{
		queueInfo[queueFamilyCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo[queueFamilyCount].queueFamilyIndex = transferQueueFamily;
		queueInfo[queueFamilyCount].queueCount = 1;
		queueInfo[queueFamilyCount].pQueuePriorities = prio + 2;
		queueFamilyCount++;
	}

	deviceInfo.queueCreateInfoCount = queueFamilyCount;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.sampleRateShading = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
	deviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	deviceFeatures.shaderClipDistance = VK_TRUE;
	deviceFeatures.shaderCullDistance = VK_TRUE;

	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = mDeviceExtensions.data();
	deviceInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
	deviceInfo.ppEnabledLayerNames = mValidationLayers.data();
	deviceInfo.pEnabledFeatures = &deviceFeatures;

	VkDevice _device;
	if (vkCreateDevice(mGpu, &deviceInfo, nullptr, &_device) != VK_SUCCESS)
		return nullptr;

	Device* device = new Device();
	device->mGpu = mGpu;
	device->mDevice = _device;
	device->mGraphicsQueueFamily = graphicsQueueFamily;
	device->mComputeQueueFamily = computeQueueFamily;
	device->mTransferQueueFamily = transferQueueFamily;
	vkGetDeviceQueue(_device, graphicsQueueFamily, graphicsQueueIndex, &device->mGraphicsQueue);
	vkGetDeviceQueue(_device, computeQueueFamily, computeQueueIndex, &device->mComputeQueue);
	vkGetDeviceQueue(_device, transferQueueFamily, transferQueueIndex, &device->mTransferQueue);

	return device;
}



GFX_NAMESPACE_END