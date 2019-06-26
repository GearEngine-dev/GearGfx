#pragma once
#include <GfxDefine.h>
#include <Device.h>
#include <vector>
GFX_NAMESPACE_BEGIN

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Context
{
public:
	Context();
	~Context();
	bool initial();
	Device* createDevice(VkSurfaceKHR surface);
	VkInstance getInstance() { return mInstance; }
private:
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
private:
	bool mInitial = false;
	std::vector<const char*> mValidationLayers;
	std::vector<const char*> mDeviceExtensions;
	VkInstance mInstance;
	VkPhysicalDevice mGPU;
};

GFX_NAMESPACE_END
