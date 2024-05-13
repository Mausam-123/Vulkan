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

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validation_layer = {
	"VK_LAYER_KHRONOS_validation"
};

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
	VkDebugUtilsMessengerEXT vk_debug_messenger;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_queue;

	void vk_create_instance(void);	//Create Instance
	void vk_register_validation_layer(void);
	void vk_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void vk_unregister_validation_layer(void);
	void vk_pick_physical_device(void);	//Select appropriate Physical Device
	QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
	VkBool32 isSuitableDevice(VkPhysicalDevice device);		//Select appropriate Physical Device
	bool vk_check_validation_layer_support(void);
	void vk_create_logical_device(void);
};
