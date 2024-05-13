#include "Vulkan.h"

vk_simple_triangle::vk_simple_triangle() {
}

vk_simple_triangle::~vk_simple_triangle() {
}

int vk_simple_triangle::vk_init(GLFWwindow* window) {
	try{
		vk_create_instance();
		vk_pick_physical_device();
		vk_create_logical_device();
	}
	catch (std::runtime_error& e) {
		std::cout << "Runtime Error : %s" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void vk_simple_triangle::vk_cleanup() {
	vkDestroyDevice(vk_device, nullptr);
	vkDestroyInstance(vk_instance, nullptr);
}

void vk_simple_triangle::vk_create_instance(void) {
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
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vk_instance);

	if (VK_SUCCESS != result) {
		throw std::runtime_error("Failed to create vk Instance");
	}
}

QueueFamilyIndices vk_simple_triangle::findQueueFamilyIndices(VkPhysicalDevice device) {
	QueueFamilyIndices indices = {};
	uint32_t QueueFamilyPropertyCount = 0;
	uint32_t i = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyPropertyCount, nullptr);

	std::vector<VkQueueFamilyProperties>vk_queues_family = std::vector<VkQueueFamilyProperties>();
	vk_queues_family.resize(QueueFamilyPropertyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyPropertyCount, vk_queues_family.data());
	
	for (auto& vk_queue : vk_queues_family) {
		if ((vk_queue.queueCount != 0) && (vk_queue.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			indices.graphicsFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}

VkBool32 vk_simple_triangle::isSuitableDevice(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilyIndices(device);
	return indices.isComplete();
}

void vk_simple_triangle::vk_pick_physical_device(void) {
	vk_physical_device = VK_NULL_HANDLE;
	uint32_t physicalDeviceCount = 0;
	
	//Get count for number of physical devices
	vkEnumeratePhysicalDevices(vk_instance, &physicalDeviceCount, nullptr);

	//Create list for holding physical devices
	std::vector<VkPhysicalDevice> physicalDevices = std::vector<VkPhysicalDevice>();
	physicalDevices.resize(physicalDeviceCount);

	//Get all the physical devices
	vkEnumeratePhysicalDevices(vk_instance, &physicalDeviceCount, physicalDevices.data());

	//Check for suitable physical devices
	for (const auto& physicalDevice : physicalDevices) {
		if (VK_TRUE == isSuitableDevice(physicalDevice)) {
			vk_physical_device = physicalDevice;
			break;
		}
	}

	if (VK_NULL_HANDLE == vk_physical_device) {
		throw std::runtime_error("Physical Device not found");
	}

	return;
}

void vk_simple_triangle::vk_create_logical_device(void) {
	QueueFamilyIndices indices = findQueueFamilyIndices(vk_physical_device);
	float QueuePriority = 1.0f;
	
	//Populate Queue Information
	VkDeviceQueueCreateInfo QueuCreateInfo = {};
	QueuCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueuCreateInfo.pNext = nullptr;
	QueuCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	QueuCreateInfo.queueCount = 1;
	QueuCreateInfo.pQueuePriorities = &QueuePriority;
	QueuCreateInfo.flags = 0;

	//Populate device related information
	VkPhysicalDeviceFeatures deviceFeatures{};

	//Populate logical device creation information
	VkDeviceCreateInfo deviceinfo {};
	deviceinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceinfo.pNext = nullptr;
	deviceinfo.flags = 0;
	deviceinfo.queueCreateInfoCount = 1;
	deviceinfo.pQueueCreateInfos = &QueuCreateInfo;
	deviceinfo.enabledExtensionCount = 0;
	deviceinfo.ppEnabledExtensionNames = nullptr;
	deviceinfo.enabledExtensionCount = 0;
	deviceinfo.ppEnabledExtensionNames = nullptr;
	deviceinfo.pEnabledFeatures = &deviceFeatures;

	VkResult result = vkCreateDevice(vk_physical_device, &deviceinfo, nullptr, &vk_device);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create logical device");
	}

	//Store the queue handle for futher submission
	vkGetDeviceQueue(vk_device, indices.graphicsFamily.value(), 0, &vk_queue);
	return;
}


