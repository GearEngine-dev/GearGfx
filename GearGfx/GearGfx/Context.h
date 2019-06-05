#pragma once
#include <GfxDefine.h>
#include <Device.h>
#include <vector>
#include <vulkan.h>
GFX_NAMESPACE_BEGIN

struct DeviceFeatures
{
	bool supports_physical_device_properties2 = false;
	bool supports_external = false;
	bool supports_dedicated = false;
	bool supports_image_format_list = false;
	bool supports_debug_marker = false;
	bool supports_debug_utils = false;
	bool supports_mirror_clamp_to_edge = false;
	bool supports_google_display_timing = false;
	bool supports_nv_device_diagnostic_checkpoints = false;
	bool supports_vulkan_11_instance = false;
	bool supports_vulkan_11_device = false;
	bool supports_external_memory_host = false;
	VkPhysicalDeviceSubgroupProperties subgroup_properties = {};
	VkPhysicalDevice8BitStorageFeaturesKHR storage_8bit_features = {};
	VkPhysicalDevice16BitStorageFeaturesKHR storage_16bit_features = {};
	VkPhysicalDeviceFeatures enabled_features = {};
	VkPhysicalDeviceExternalMemoryHostPropertiesEXT host_memory_properties = {};
};

class Context
{
public:
	Context();
	~Context();
	bool initial(const char **instanceExt, uint32_t instanceExtCount);
	Device* createDevice(const char **deviceExt, uint32_t deviceExtCount);
private:
	bool mInitial = false;
	DeviceFeatures mExt;
	VkInstance mInstance;
	VkPhysicalDevice mGpu;
};
GFX_NAMESPACE_END
