#include "CommandBuffer.h"
#include "Device.h"
GFX_NAMESPACE_BEGIN

CommandBuffer::CommandBuffer(Device* device)
	:mDevice(device)
{
}

CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool, 1, &mCommandBuffer);
}
GFX_NAMESPACE_END