#include<iostream>
#include<Context.h>
int main()
{
	gfx::Context c;
	c.initInstanceAndDevice(nullptr,0,nullptr,0);
	getchar();
	return 0;
}