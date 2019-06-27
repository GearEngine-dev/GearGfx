#include <iostream>
#include <Context.h>
#include <SwapChain.h>
#define WIDTH 800
#define HEIGHT 600
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Gear", NULL, NULL);

	gfx::Context context;
	context.initial();
	VkSurfaceKHR surface;
	auto res = glfwCreateWindowSurface(context.getInstance(), window, nullptr, &surface);
	if (res != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	gfx::Device* device = context.createDevice(surface);
	gfx::SwapChain* swapchain = new gfx::SwapChain(device);
	VkExtent2D exten;
	exten.width = WIDTH;
	exten.height = HEIGHT;
	swapchain->initial(exten);
	
	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	delete swapchain;
	delete device;
	getchar();
	return 0;
}