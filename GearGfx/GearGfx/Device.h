#pragma once
#include <GfxDefine.h>
#include <memory>
#include <vulkan.h>
GFX_NAMESPACE_BEGIN
class Context;
class Device
{
public:
	Device();
	virtual~Device();

private:
	friend class Context;
	void create();
	VkPhysicalDevice mGpu;
};


GFX_NAMESPACE_END
