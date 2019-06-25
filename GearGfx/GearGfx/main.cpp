#include <iostream>
#include <Context.h>
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Gear", NULL, NULL);
	glfwSetWindowUserPointer(window, nullptr);
	gfx::Context context;
	context.initial();
	VkSurfaceKHR surface;
	auto res = glfwCreateWindowSurface(context.getInstance(), window, nullptr, &surface);
	if (res != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	gfx::Device* device = context.createDevice(surface);

	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();

		//glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	getchar();
	return 0;
}