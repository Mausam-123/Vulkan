#include "Vulkan.h"

void vk_simple_triangle::vk_create_sync_objects() {
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if ((vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &image_available_sync) != VK_SUCCESS) ||
		(vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &render_finish_sync) != VK_SUCCESS) ||
		(vkCreateFence(vk_device, &fence_info, nullptr, &frame_render_sync) != VK_SUCCESS)) {
		throw std::runtime_error("Unable to create Sync objects");
	}
}