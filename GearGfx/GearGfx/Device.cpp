#include <Device.h>

GFX_NAMESPACE_BEGIN

Device::Device()
{
	
}
Device::~Device()
{
	vkDestroyDevice(mDevice, nullptr);
}

GFX_NAMESPACE_END