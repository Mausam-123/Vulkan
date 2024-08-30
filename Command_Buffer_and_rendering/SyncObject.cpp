#include "Vulkan.h"

void vk_simple_triangle::vk_create_sync_objects() {
	image_available_sync.resize(MAX_NUMBER_OF_FRAMES_INFLIGHT);
	render_finish_sync.resize(MAX_NUMBER_OF_FRAMES_INFLIGHT);
	frame_render_sync.resize(MAX_NUMBER_OF_FRAMES_INFLIGHT);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_NUMBER_OF_FRAMES_INFLIGHT; i++) {
		if ((vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &image_available_sync[i]) != VK_SUCCESS) ||
			(vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &render_finish_sync[i]) != VK_SUCCESS) ||
			(vkCreateFence(vk_device, &fence_info, nullptr, &frame_render_sync[i]) != VK_SUCCESS)) {
			throw std::runtime_error("Unable to create Sync objects");
		}
	}
}