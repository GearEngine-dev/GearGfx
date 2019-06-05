#include<iostream>
#include<Context.h>
int main()
{
	gfx::Context c;
	c.initial(nullptr,0);
	gfx::Device* d = c.createDevice(nullptr,0);
	delete d;
	getchar();
	return 0;
}