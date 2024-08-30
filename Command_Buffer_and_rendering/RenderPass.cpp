#include "Vulkan.h"

void vk_simple_triangle::vk_create_render_pass(void) {
	VkAttachmentDescription attachment_info = {};
	attachment_info.flags = 0;
	attachment_info.format = swapchain_image_format;
	attachment_info.samples = VK_SAMPLE_COUNT_1_BIT;	//Avoid Multisampling
	attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_info.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_info.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_info.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachment_ref = {};
	attachment_ref.attachment = 0;
	attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_info = {};
	subpass_info.flags = 0;
	subpass_info.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		//Pipeline type supported by this pass
	subpass_info.colorAttachmentCount = 1;
	subpass_info.pColorAttachments = &attachment_ref;
	subpass_info.inputAttachmentCount = 0;
	subpass_info.pInputAttachments = nullptr;
	subpass_info.preserveAttachmentCount = 0;
	subpass_info.pPreserveAttachments = nullptr;
	subpass_info.pDepthStencilAttachment = nullptr;
	
	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency.dependencyFlags = 0;

	VkRenderPassCreateInfo vk_renderpass_info = {};
	vk_renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	vk_renderpass_info.pNext = nullptr;
	vk_renderpass_info.attachmentCount = 1;
	vk_renderpass_info.pAttachments = &attachment_info;
	vk_renderpass_info.subpassCount = 1;
	vk_renderpass_info.pSubpasses = &subpass_info;
	vk_renderpass_info.dependencyCount = 1;
	vk_renderpass_info.pDependencies = &subpass_dependency;


	VkResult result = vkCreateRenderPass(vk_device, &vk_renderpass_info, nullptr, &vk_render_pass);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create renderpass");
	}
}

void vk_simple_triangle::vk_create_framebuffer(void) {
	swapchain_framebuffer.resize(swapchain_image_view.size());

	//Create framebuffer for every imageview

	for (size_t i = 0; i < swapchain_image_view.size(); i++) {
		VkImageView imageview_ref[] = { swapchain_image_view[i] };
		
		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.pNext = nullptr;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = imageview_ref;
		framebuffer_info.renderPass = vk_render_pass;
		framebuffer_info.flags = 0;
		framebuffer_info.width = swapchain_extent.width;
		framebuffer_info.height = swapchain_extent.height;
		framebuffer_info.layers = 1;			//Used for multiview

		VkResult result = vkCreateFramebuffer(vk_device, &framebuffer_info, nullptr, &swapchain_framebuffer[i]);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Unable to create framebuffer at index");
		}
	}

}