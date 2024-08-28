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
#include <set>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validation_layer = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> device_extensions = {
	"VK_KHR_swapchain"
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentQueue;
	std::optional<uint32_t> transferQueue;

	bool isComplete() {
		return (graphicsFamily.has_value() && presentQueue.has_value() && transferQueue.has_value());
	}
};

struct SwapChainSupport {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formates;
	std::vector<VkPresentModeKHR> presentModes;
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
	VkSurfaceKHR vk_surface;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_graphics_queue;
	VkQueue vk_present_queue;
	VkQueue vk_transfer_queue;

	//Swapchain realted structure
	VkSwapchainKHR vk_swapchain;
	std::vector<VkImage> swapchain_images;
	VkExtent2D swapchain_extent;
	VkFormat swapchain_image_format;
	std::vector<VkImageView> swapchain_image_view;

	//Render Pass Variable
	VkRenderPass vk_render_pass;

	//Pipeline 
	VkShaderModule vertex_shader_module;
	VkShaderModule frag_shader_module;
	VkPipelineLayout vk_pipelineLayout;
	VkPipeline vk_graphicsPipeline;

	void vk_create_instance(void);	//Create Instance
	void vk_register_validation_layer(void);
	void vk_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void vk_unregister_validation_layer(void);
	void vk_create_surface(GLFWwindow* window);
	void vk_pick_physical_device(void);	//Select appropriate Physical Device
	QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
	VkBool32 isDeviceSupportSwapchain(VkPhysicalDevice device);
	VkBool32 isSuitableDevice(VkPhysicalDevice device);		//Select appropriate Physical Device
	bool vk_check_validation_layer_support(void);
	void vk_create_logical_device(void);
	
	//Functions related to swapchain
	SwapChainSupport vk_query_swapchain_support(VkPhysicalDevice device);
	VkSurfaceFormatKHR choose_swapchain_format(const std::vector<VkSurfaceFormatKHR> &Availableformats);
	VkPresentModeKHR choose_swapchain_present_mode(const std::vector<VkPresentModeKHR>& presentModes);
	VkExtent2D choose_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR capabilities);
	void vk_create_swapchain(GLFWwindow* window);
	void vk_create_swapchain_image_view(void);
	
	//Functions to create Pipeline
	void vk_create_pipeline(void);
	VkShaderModule create_shader_module(const std::vector<char>& code);

	//Function to create Render Pass
	void vk_create_render_pass(void);
};
