#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include <iostream>

class vk_simple_triangle {
public:
	vk_simple_triangle();
	int vk_init(GLFWwindow* window);
	void vk_cleanup();

	~vk_simple_triangle();
private:
	VkInstance vk_instance;

	int vk_create_instance(void);	//Create Instance
};
