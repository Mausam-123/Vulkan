#include "Vulkan.h"

void vk_simple_triangle::vk_create_command_pool() {
	VkCommandPoolCreateInfo command_pool_info = {};
	command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_info.pNext = nullptr;
	//Create a graphics pool
	command_pool_info.queueFamilyIndex = static_cast<uint32_t>(findQueueFamilyIndices(vk_physical_device).graphicsFamily.value());
	command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(vk_device, &command_pool_info, nullptr, &vk_command_pool);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create command pool");
	}
}

void vk_simple_triangle::vk_allocate_command_buffer() {
	vk_command_buffer.resize(MAX_NUMBER_OF_FRAMES_INFLIGHT);

	VkCommandBufferAllocateInfo command_buf_alloc_info = {};
	command_buf_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buf_alloc_info.pNext = nullptr;
	command_buf_alloc_info.commandBufferCount = static_cast<uint32_t>(vk_command_buffer.size());
	command_buf_alloc_info.commandPool = vk_command_pool;
	command_buf_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkResult result = vkAllocateCommandBuffers(vk_device, &command_buf_alloc_info, vk_command_buffer.data());

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to allocate command buffer");
	}
	//Command Buffer state : INITIAL
	vk_command_buffer_state = VK_COMMAND_BUFFER_INITIAL;
}

void vk_simple_triangle::vk_record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index) {
	VkResult result = VK_ERROR_UNKNOWN;

	//Put the command buffer into RECORDING STATE
	VkCommandBufferBeginInfo command_buf_begin_info = {};
	command_buf_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buf_begin_info.pNext = nullptr;
	command_buf_begin_info.pInheritanceInfo = 0;
	command_buf_begin_info.flags = 0;

	result = vkBeginCommandBuffer(command_buffer, &command_buf_begin_info);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to start recording command buffer");
	}
	//Command Buffer State : INITIAL -> RECORDING
	vk_command_buffer_state = VK_COMMAND_BUFFER_RECORDING;

	//Now command buffer is ready to accept command that will be executed on hardware

	VkClearValue clear_value_info = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

	VkRect2D vk_rect_info = {};
	vk_rect_info.extent = swapchain_extent;
	vk_rect_info.offset = { 0, 0 };

	//Start Render Pass
	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.pNext = nullptr;
	render_pass_begin_info.framebuffer = swapchain_framebuffer[image_index];
	render_pass_begin_info.renderPass = vk_render_pass;
	render_pass_begin_info.clearValueCount = 1;
	render_pass_begin_info.pClearValues = &clear_value_info;	// Define the clear values to use for VK_ATTACHMENT_LOAD_OP_CLEAR
	render_pass_begin_info.renderArea = vk_rect_info;

	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	//Now Bind the buffers to pipeline
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphicsPipeline);

	//Scissor and ViewPort are set as dynamic so need to configure here before issuing a command to GPU
	VkViewport view_port_info = {};
	view_port_info.x = 0.0f;
	view_port_info.y = 0.0f;
	view_port_info.height = (float)swapchain_extent.height;
	view_port_info.width = (float)swapchain_extent.width;
	view_port_info.minDepth = 0.0f;
	view_port_info.maxDepth = 1.0f;

	vkCmdSetViewport(command_buffer, 0, 1, &view_port_info);

	VkRect2D scissor_info = {};
	scissor_info.offset = { 0, 0 };
	scissor_info.extent = swapchain_extent;

	vkCmdSetScissor(command_buffer, 0, 1, &scissor_info);

	//Now issue draw command
	vkCmdDraw(command_buffer, 3, 1, 0, 0);

	//End Render Pass
	vkCmdEndRenderPass(command_buffer);

	//Now End the recording and put the command buffer into executable state
	result = vkEndCommandBuffer(command_buffer);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to end command buffer recording");
	}
	//Command Buffer state : RECORDING -> EXECUTABLE
	vk_command_buffer_state = VK_COMMAND_BUFFER_EXECUTABLE;
}

void vk_simple_triangle::vk_draw_frame_on_screen() {
	uint32_t available_image_index;

	//Wait for previous frame to finish rendering
	vkWaitForFences(vk_device, 1, &frame_render_sync[vk_current_frame], VK_TRUE, UINT64_MAX);
	vkResetFences(vk_device, 1, &frame_render_sync[vk_current_frame]);

	//Acquire the next frame once available
	vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, image_available_sync[vk_current_frame], VK_NULL_HANDLE, &available_image_index);

	//Once image is available draw and submit to Queue
	vkResetCommandBuffer(vk_command_buffer[vk_current_frame], 0);
	vk_record_command_buffer(vk_command_buffer[vk_current_frame], available_image_index);

	VkSemaphore wait_semaphore [] = { image_available_sync[vk_current_frame] };
	VkSemaphore signal_semaphore [] = { render_finish_sync[vk_current_frame] };
	VkPipelineStageFlags pipeline_wait_stage[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submit_command_buffer = {};
	submit_command_buffer.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_command_buffer.pNext = nullptr;
	submit_command_buffer.commandBufferCount = 1;
	submit_command_buffer.pCommandBuffers = &vk_command_buffer[vk_current_frame];
	submit_command_buffer.waitSemaphoreCount = 1;
	submit_command_buffer.pWaitSemaphores = wait_semaphore;
	submit_command_buffer.pWaitDstStageMask = pipeline_wait_stage;
	submit_command_buffer.signalSemaphoreCount = 1;
	submit_command_buffer.pSignalSemaphores = signal_semaphore;

	VkResult result = vkQueueSubmit(vk_graphics_queue, 1, &submit_command_buffer, frame_render_sync[vk_current_frame]);
	//Once processing is done it will signal "frame_render_sync" fence

	VkSwapchainKHR swap_chains[] = { vk_swapchain };

	//Present frame on screen
	VkPresentInfoKHR present_image = {};
	present_image.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_image.pNext = nullptr;
	present_image.swapchainCount = 1;
	present_image.pSwapchains = swap_chains;
	present_image.waitSemaphoreCount = 1;
	present_image.pWaitSemaphores = signal_semaphore;
	present_image.pImageIndices = &available_image_index;
	present_image.pResults = nullptr;

	vkQueuePresentKHR(vk_present_queue, &present_image);

	vk_current_frame = (vk_current_frame + 1) % MAX_NUMBER_OF_FRAMES_INFLIGHT;
}