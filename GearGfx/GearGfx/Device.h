#pragma once
#include "GfxDefine.h"
#include "CommandBuffer.h"
#include <memory>
GFX_NAMESPACE_BEGIN
class Context;
class Device
{
public:
	Device();
	virtual~Device();

	CommandBuffer* allocCommandBuffer(const CommandBufferType& type, const CommandBufferLevel& level);

	VkDevice getDevice()
	{
		return mDevice;
	}
	VkPhysicalDevice getGPU()
	{
		return mGPU;
	}
	VkSurfaceKHR getSurface()
	{
		return mSurface;
	}
private:
	void createCommandPool();
private:
	friend class Context;
	VkPhysicalDevice mGPU;
	VkDevice mDevice;
	VkSurfaceKHR mSurface;
	VkCommandPool mGraphicsCommandPool;
	VkCommandPool mComputeCommandPool;
	VkCommandPool mTransferCommandPool;
	VkQueue mGraphicsQueue = VK_NULL_HANDLE;
	VkQueue mComputeQueue = VK_NULL_HANDLE;
	VkQueue mTransferQueue = VK_NULL_HANDLE;
	uint32_t mGraphicsQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	uint32_t mComputeQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	uint32_t mTransferQueueFamily = VK_QUEUE_FAMILY_IGNORED;
};


GFX_NAMESPACE_END
