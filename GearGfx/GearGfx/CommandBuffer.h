#pragma once
#include "GfxDefine.h"
GFX_NAMESPACE_BEGIN
class Device;
enum class CommandBufferType
{ 
	Graphics,
	Compute,
	Transfer
};

enum class CommandBufferLevel
{
	Primary,
	Secondary
};

class CommandBuffer
{
public:
	CommandBuffer(Device* device);
	~CommandBuffer();
	
private:
	friend class Device;
	Device* mDevice;
	VkCommandBuffer mCommandBuffer;
	VkCommandPool mCommandPool;
	CommandBufferType mType;
	CommandBufferLevel mLevel;
};
GFX_NAMESPACE_END