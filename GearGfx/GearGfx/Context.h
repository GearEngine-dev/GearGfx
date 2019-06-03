#pragma once
#include <GfxDefine.h>
#include <Device.h>
#include <vector>
GFX_NAMESPACE_BEGIN
class Context
{
public:
	Context();
	~Context();
	bool initInstanceAndDevice(const char **instanceExt, uint32_t instanceExtCount, const char **deviceExt,uint32_t deviceExtCount);
private:
	
};
GFX_NAMESPACE_END
