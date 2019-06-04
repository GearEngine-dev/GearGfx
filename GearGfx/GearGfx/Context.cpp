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

#ifdef VULKAN_DEBUG
	instanceExts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = instanceExts.size();;
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
		//todo
		return nullptr;
	}
		
	return nullptr;
}



GFX_NAMESPACE_END