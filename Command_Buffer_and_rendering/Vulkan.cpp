#include "Vulkan.h"

vk_simple_triangle::vk_simple_triangle() {
}

vk_simple_triangle::~vk_simple_triangle() {
}

int vk_simple_triangle::vk_init(GLFWwindow* window) {
	try{
		vk_create_instance();
		vk_register_validation_layer();
		vk_create_surface(window); //Create it before physical surface as it can influence phyiscal properties
		vk_pick_physical_device();
		vk_create_logical_device();
		vk_create_swapchain(window);
		vk_create_swapchain_image_view();
		vk_create_render_pass();
		vk_create_pipeline();
		vk_create_framebuffer();
		vk_create_command_pool();
		vk_allocate_command_buffer();
		vk_create_sync_objects();
	}
	catch (std::runtime_error& e) {
		std::cout << "Runtime Error : %s" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void vk_simple_triangle::vk_cleanup() {
	//Destroy Sync objects
	for (size_t i = 0; i < MAX_NUMBER_OF_FRAMES_INFLIGHT; i++) {
		vkDestroySemaphore(vk_device, image_available_sync[i], nullptr);
		vkDestroySemaphore(vk_device, render_finish_sync[i], nullptr);
		vkDestroyFence(vk_device, frame_render_sync[i], nullptr);
	}

	//Destroy Command Pool
	vkDestroyCommandPool(vk_device, vk_command_pool, nullptr);

	//Destroy Framebuffer
	for (size_t i = 0; i < swapchain_framebuffer.size(); i++) {
		vkDestroyFramebuffer(vk_device, swapchain_framebuffer[i], nullptr);
	}

	//Destroy pipeline
	vkDestroyPipeline(vk_device, vk_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vk_device, vk_pipelineLayout, nullptr);
	vkDestroyRenderPass(vk_device, vk_render_pass, nullptr);
	vkDestroyShaderModule(vk_device, vertex_shader_module, nullptr);
	vkDestroyShaderModule(vk_device, frag_shader_module, nullptr);

	//Destroy Image View
	for (auto& i : swapchain_image_view) {
		vkDestroyImageView(vk_device, i, nullptr);
	}
	//Destroy swapchain
	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);

	//Destroy logical device
	vkDestroyDevice(vk_device, nullptr);

	//Destory surface
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);

	//Unregister validation layer
	if (enableValidationLayers) {
		vk_unregister_validation_layer();
	}

	//Destroy instance
	vkDestroyInstance(vk_instance, nullptr);
}

bool vk_simple_triangle::vk_check_validation_layer_support(void) {
	uint32_t layerCount = 0;
	bool layerfound = false;
	bool ret = false;
	std::vector<VkLayerProperties>validation_layer_names = std::vector <VkLayerProperties>();

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	validation_layer_names.resize(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, validation_layer_names.data());

	for (auto& support_layer_name : validation_layer) {
		for (auto& validation_layer_name : validation_layer_names) {
			if (strcmp(support_layer_name, validation_layer_name.layerName)) {
				layerfound = true;
			}
		}

		if (layerfound) {
			ret = true;
		}
	}

	return ret;
}

void vk_simple_triangle::vk_create_instance(void) {
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; //Validation Layer related information
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

	vk_check_validation_layer_support();
	//Get all the glfw extension
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames;
	
	glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//Push all the extnsion to the list
	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		instanceExtension.push_back(glfwExtensionNames[i]);
	}

	//For printing the message on the console
	if (enableValidationLayers) {
		instanceExtension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	//Populate instance related information
	VkInstanceCreateInfo instanceInfo{};

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	if (enableValidationLayers && vk_check_validation_layer_support()) {
		instanceInfo.enabledLayerCount = validation_layer.size();
		instanceInfo.ppEnabledLayerNames = validation_layer.data();
		vk_populateDebugMessengerCreateInfo(debugCreateInfo);
		instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
	}
	//instanceInfo.enabledLayerCount = 0;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtension.data();
	
	//Create VK Instance 
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vk_instance);

	if (VK_SUCCESS != result) {
		throw std::runtime_error("Failed to create vk Instance");
	}
}

void vk_simple_triangle::vk_create_surface(GLFWwindow* window) {
	VkResult result = glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create Window Surface");
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
		if (vk_queue.queueCount != 0) {
			if (vk_queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (vk_queue.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				indices.transferQueue = i;
			}

			VkBool32 supportPresent = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface, &supportPresent);

			if (supportPresent) {
				indices.presentQueue = i;
			}
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}

VkBool32 vk_simple_triangle::isSuitableDevice(VkPhysicalDevice device) {
	SwapChainSupport details = {};
	VkBool32 SwapchainAdequate = VK_FALSE;
	QueueFamilyIndices indices = findQueueFamilyIndices(device);
	VkBool32 swapchain_extesnion = isDeviceSupportSwapchain(device);
	
	if (swapchain_extesnion) {
		details = vk_query_swapchain_support(device);
		SwapchainAdequate = (!details.formates.empty() && !details.presentModes.empty());
	}

	return (indices.isComplete() && swapchain_extesnion && SwapchainAdequate);
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
	
	std::vector<VkDeviceQueueCreateInfo> QueuesInfo = std::vector<VkDeviceQueueCreateInfo>();
	
	std::set<uint32_t> QueueIndices = std::set<uint32_t>();
	QueueIndices.insert(indices.graphicsFamily.value());
	QueueIndices.insert(indices.presentQueue.value());
	QueueIndices.insert(indices.transferQueue.value());

	for (auto& QueueIndice : QueueIndices) {
		//Populate Queue Information
		VkDeviceQueueCreateInfo QueuCreateInfo = {};
		QueuCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueuCreateInfo.pNext = nullptr;
		QueuCreateInfo.queueFamilyIndex = QueueIndice;
		QueuCreateInfo.queueCount = 1;
		QueuCreateInfo.pQueuePriorities = &QueuePriority;
		QueuCreateInfo.flags = 0;
		QueuesInfo.push_back(QueuCreateInfo);
	}

	//Populate device related information
	VkPhysicalDeviceFeatures deviceFeatures{};

	//Populate logical device creation information
	VkDeviceCreateInfo deviceinfo {};
	deviceinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceinfo.pNext = nullptr;
	deviceinfo.flags = 0;
	deviceinfo.queueCreateInfoCount = QueuesInfo.size();
	deviceinfo.pQueueCreateInfos = QueuesInfo.data();
	deviceinfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	deviceinfo.ppEnabledExtensionNames = device_extensions.data();
	deviceinfo.pEnabledFeatures = &deviceFeatures;

	VkResult result = vkCreateDevice(vk_physical_device, &deviceinfo, nullptr, &vk_device);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create logical device");
	}

	//Store the queue handle for futher submission
	vkGetDeviceQueue(vk_device, indices.graphicsFamily.value(), 0, &vk_graphics_queue);
	vkGetDeviceQueue(vk_device, indices.presentQueue.value(), 0, &vk_present_queue);
	vkGetDeviceQueue(vk_device, indices.transferQueue.value(), 0, &vk_transfer_queue);
	return;
}


