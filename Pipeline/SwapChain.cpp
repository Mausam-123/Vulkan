#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include "Vulkan.h"

VkBool32 vk_simple_triangle::isDeviceSupportSwapchain(VkPhysicalDevice device) {
	uint32_t device_extension_count = 0;
	std::vector<VkExtensionProperties> query_device_extensions = std::vector<VkExtensionProperties>();
	VkBool32 extension_found = VK_FALSE;

	vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, nullptr);
	query_device_extensions.resize(device_extension_count);

	vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, query_device_extensions.data());

	for (auto& device_extension : device_extensions) {

		for (auto& query_device_extension : query_device_extensions) {
			if (strcmp(query_device_extension.extensionName, device_extension)) {
				extension_found = VK_TRUE;
			}
		}

		if (extension_found)
			break;
	}

	return (extension_found ? VK_TRUE : VK_FALSE);
}

SwapChainSupport vk_simple_triangle::vk_query_swapchain_support(VkPhysicalDevice device) {
	SwapChainSupport details;
	uint32_t surface_formate_count = 0;
	uint32_t present_mode_count = 0;

	//Query Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface, &details.capabilities);

	//Query Surface Formate
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &surface_formate_count, nullptr);
	if (surface_formate_count != 0) {
		details.formates.resize(surface_formate_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &surface_formate_count, details.formates.data());
	}

	//Query Presentation Mode
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, nullptr);
	if (present_mode_count != 0) {
		details.presentModes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, details.presentModes.data());
	}
	return details;
}

VkSurfaceFormatKHR vk_simple_triangle::choose_swapchain_format(const std::vector<VkSurfaceFormatKHR>& Availableformats) {
	for (const auto& Availableformat : Availableformats) {
		if ((Availableformat.format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(Availableformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			return Availableformat;
		}
	}
	return Availableformats[0];
}

VkPresentModeKHR vk_simple_triangle::choose_swapchain_present_mode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vk_simple_triangle::choose_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int height = 0;
		int width = 0;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D dimension = { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		dimension.width = std::clamp(dimension.width, capabilities.minImageExtent.width, 
									capabilities.maxImageExtent.width);
		dimension.height = std::clamp(dimension.height, capabilities.minImageExtent.height,
									capabilities.maxImageExtent.height);
		return dimension;
	}
}

void vk_simple_triangle::vk_create_swapchain(GLFWwindow* window) {
	SwapChainSupport details = vk_query_swapchain_support(vk_physical_device);

	VkSurfaceFormatKHR swapchain_format = choose_swapchain_format(details.formates);
	VkPresentModeKHR swapchain_present_mode = choose_swapchain_present_mode(details.presentModes);
	VkExtent2D swapchain_image_extent = choose_swap_extent(window, details.capabilities);

	uint32_t minImageCount = details.capabilities.minImageCount + 1;

	if (minImageCount > details.capabilities.maxImageCount) {
		minImageCount = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.imageFormat = swapchain_format.format;
	swapchainInfo.imageColorSpace = swapchain_format.colorSpace;
	swapchainInfo.presentMode = swapchain_present_mode;
	swapchainInfo.imageExtent = swapchain_image_extent;
	swapchainInfo.surface = vk_surface;
	swapchainInfo.minImageCount = minImageCount;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices queueindices = findQueueFamilyIndices(vk_physical_device);
	std::vector<uint32_t> indices = std::vector<uint32_t>();
	indices.push_back(queueindices.graphicsFamily.value());
	indices.push_back(queueindices.presentQueue.value());
	indices.push_back(queueindices.transferQueue.value());

	if ((queueindices.graphicsFamily.value() != queueindices.presentQueue.value()) &&
		(queueindices.transferQueue.value() != queueindices.presentQueue.value())) {
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainInfo.queueFamilyIndexCount = indices.size();
		swapchainInfo.pQueueFamilyIndices = indices.data();
	}
	else {
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.queueFamilyIndexCount = 0; //optional
		swapchainInfo.pQueueFamilyIndices = nullptr; //optional
	}

	swapchainInfo.preTransform = details.capabilities.currentTransform;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(vk_device, &swapchainInfo, nullptr, &vk_swapchain);
	
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create swapchain");
	}

	//Retrive the swapchain images
	uint32_t swapchain_image_count = 0;

	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &swapchain_image_count, nullptr);
	swapchain_images.resize(swapchain_image_count);
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &swapchain_image_count, swapchain_images.data());
	swapchain_extent = swapchain_image_extent;
	swapchain_image_format = swapchain_format.format;
}

void vk_simple_triangle::vk_create_swapchain_image_view(void) {
	swapchain_image_view.resize(swapchain_images.size());

	for (size_t i = 0; i < swapchain_images.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.format = swapchain_image_format;
		createInfo.image = swapchain_images[i];
		createInfo.flags = 0;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;

		VkResult result = vkCreateImageView(vk_device, &createInfo, nullptr, &swapchain_image_view[i]);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Unable to create Image view for index");
		}
	}
}