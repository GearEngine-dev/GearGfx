#include<Context.h>
#include <algorithm>
GFX_NAMESPACE_BEGIN
Context::Context()
{
	printf("hi");
}

Context::~Context()
{
	vkDestroyInstance(mInstance, nullptr);
}

bool Context::initial(const char ** instanceExt, uint32_t instanceExtCount)
{
	std::vector<const char *> instanceExts;
	std::vector<const char *> instanceLayers;

	for (uint32_t i = 0; i < instanceExtCount; i++)
		instanceExts.push_back(instanceExt[i]);

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Gear";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Gear Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
	std::vector<VkExtensionProperties> queriedExtensions(extCount);
	if (extCount)
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, queriedExtensions.data());

	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> queriedLayers(layerCount);
	if (layerCount)
		vkEnumerateInstanceLayerProperties(&layerCount, queriedLayers.data());

	const auto hasExtension = [&](const char *name) -> bool 
	{
		auto itr = std::find_if(std::begin(queriedExtensions), std::end(queriedExtensions), [name](const VkExtensionProperties &e) -> bool
		{
			return strcmp(e.extensionName, name) == 0;
		});
		return itr != std::end(queriedExtensions);
	};

	for (uint32_t i = 0; i < instanceExtCount; i++)
		if (!hasExtension(instanceExt[i]))
			return false;

	if (hasExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
	{
		mExt.supports_physical_device_properties2 = true;
		instanceExts.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	}

	if (mExt.supports_physical_device_properties2 &&
		hasExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME) &&
		hasExtension(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME))
	{
		instanceExts.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		instanceExts.push_back(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
		instanceExts.push_back(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
		mExt.supports_external = true;
	}

	if (hasExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
	{
		instanceExts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		mExt.supports_debug_utils = true;
	}

#ifdef VULKAN_DEBUG
	const auto hasLayer = [&](const char *name) -> bool 
	{
		auto itr = std::find_if(std::begin(queriedLayers), std::end(queriedLayers), [name](const VkLayerProperties &e) -> bool 
		{
			return strcmp(e.layerName, name) == 0;
		});
		return itr != std::end(queriedLayers);
	};

	if (!mExt.supports_debug_utils && hasExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
		instanceExts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	if (hasLayer("VK_LAYER_LUNARG_standard_validation"))
		instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = instanceExts.size();
	createInfo.ppEnabledExtensionNames = instanceExts.empty() ? nullptr : instanceExts.data();;
	createInfo.enabledLayerCount = instanceLayers.size();
	createInfo.ppEnabledLayerNames = instanceLayers.empty() ? nullptr : instanceLayers.data();

	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}

	mInitial = true;
	return true;
}

Device * Context::createDevice(const char ** deviceExt, uint32_t deviceExtCount)
{
	if (!mInitial)
	{
		throw std::runtime_error("must to create instance!");
		return nullptr;
	}
	
	uint32_t gpuCount = 0;
	if (vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to find gpu!");
		return nullptr;
	}

	if (gpuCount == 0)
	{
		throw std::runtime_error("failed to find gpu!");
		return nullptr;
	}

	std::vector<VkPhysicalDevice> gpus(gpuCount);
	if (vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpus.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to find gpu!");
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

	Device* device = new Device();
	device->mGpu = mGpu;
	return device;
}



GFX_NAMESPACE_END