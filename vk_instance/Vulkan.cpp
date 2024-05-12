#include "Vulkan.h"

vk_simple_triangle::vk_simple_triangle() {
}

vk_simple_triangle::~vk_simple_triangle() {
}

int vk_simple_triangle::vk_init(GLFWwindow* window) {
	try{
		vk_create_instance();
	}
	catch (std::runtime_error& e) {
		std::cout << "Runtime Error : %s" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void vk_simple_triangle::vk_cleanup() {
	vkDestroyInstance(vk_instance, nullptr);
}

int vk_simple_triangle::vk_create_instance(void) {
	VkResult result;
	//Populate application related information
	VkApplicationInfo appInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Simple Triangle Application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Unreal Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Create the list to hold the instance extension
	std::vector<const char*> instanceExtension = std::vector<const char*>();

	//Get all the glfw extension
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames;
	
	glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Push all the extnsion to the list
	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		instanceExtension.push_back(glfwExtensionNames[i]);
	}
	//Populate instance related information
	VkInstanceCreateInfo instanceInfo{};

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtension.data();
	
	//Create VK Instance 
	result = vkCreateInstance(&instanceInfo, nullptr, &vk_instance);

	if (VK_SUCCESS != result) {
		throw std::runtime_error("Failed to create vk Instance");
	}
	return result;
}



