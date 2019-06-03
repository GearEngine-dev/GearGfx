#include<Context.h>
GFX_NAMESPACE_BEGIN
Context::Context()
{
	printf("hi");
}

Context::~Context()
{
}
bool Context::initInstanceAndDevice(const char ** instanceExt, uint32_t instanceExtCount, const char ** deviceExt, uint32_t deviceExtCount)
{
	return false;
}
GFX_NAMESPACE_END