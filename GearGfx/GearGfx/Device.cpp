#include <Device.h>
#include <stdexcept>

GFX_NAMESPACE_BEGIN

Device::Device()
{
	
}
Device::~Device()
{
	//必须保证在请空前所有命令已经销毁
	vkDestroyCommandPool(mDevice, mGraphicsCommandPool, nullptr);
	vkDestroyCommandPool(mDevice, mComputeCommandPool, nullptr);
	vkDestroyCommandPool(mDevice, mTransferCommandPool, nullptr);
	vkDestroyDevice(mDevice, nullptr);
}

CommandBuffer * Device::allocCommandBuffer(const CommandBufferType& type, const CommandBufferLevel& level)
{
	CommandBuffer* commandBuffer = new CommandBuffer(this);

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	if (type == CommandBufferType::Graphics)
	{
		commandBuffer->mCommandPool = mGraphicsCommandPool;
		allocateInfo.commandPool = mGraphicsCommandPool;
	}
	else if (type == CommandBufferType::Compute)
	{
		commandBuffer->mCommandPool = mComputeCommandPool;
		allocateInfo.commandPool = mComputeCommandPool;
	}
	else
	{
		commandBuffer->mCommandPool = mTransferCommandPool;
		allocateInfo.commandPool = mTransferCommandPool;
	}

	if (level == CommandBufferLevel::Primary)
	{
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	}
	else
	{
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	}
	
	allocateInfo.commandBufferCount = 1;

	commandBuffer->mType = type;
	commandBuffer->mLevel = level;

	if (vkAllocateCommandBuffers(mDevice, &allocateInfo, &commandBuffer->mCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer!");
	}
	return commandBuffer;
}

void Device::createCommandPool()
{
	//图形命令池和计算命令池是可重复使用
	VkCommandPoolCreateInfo graphicsPoolInfo = {};
	graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicsPoolInfo.queueFamilyIndex = mGraphicsQueueFamily;
	graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(mDevice, &graphicsPoolInfo, nullptr, &mGraphicsCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics command pool!");
	}

	VkCommandPoolCreateInfo computePoolInfo = {};
	computePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	computePoolInfo.queueFamilyIndex = mComputeQueueFamily;
	computePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(mDevice, &computePoolInfo, nullptr, &mComputeCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create compute command pool!");
	}
	VkCommandPoolCreateInfo transferPoolInfo = {};
	transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferPoolInfo.queueFamilyIndex = mTransferQueueFamily;
	transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	if (vkCreateCommandPool(mDevice, &transferPoolInfo, nullptr, &mTransferCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create transfer command pool!");
	}
}

GFX_NAMESPACE_END