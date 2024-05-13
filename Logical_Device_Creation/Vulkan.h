#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

class vk_simple_triangle {
public:
	vk_simple_triangle();
	int vk_init(GLFWwindow* window);
	void vk_cleanup();

	~vk_simple_triangle();
private:
	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_queue;

	void vk_create_instance(void);	//Create Instance
	void vk_pick_physical_device(void);	//Select appropriate Physical Device
	QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
	VkBool32 isSuitableDevice(VkPhysicalDevice device);		//Select appropriate Physical Device
	void vk_create_logical_device(void);
};
