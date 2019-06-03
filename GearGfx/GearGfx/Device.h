#pragma once
#include <GfxDefine.h>
GFX_NAMESPACE_BEGIN
class Device
{
public:
	Device();
	virtual~Device();

private:
	void create();
};

GFX_NAMESPACE_END
