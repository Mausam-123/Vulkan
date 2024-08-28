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

	VkRenderPassCreateInfo vk_renderpass_info = {};
	vk_renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	vk_renderpass_info.pNext = nullptr;
	vk_renderpass_info.attachmentCount = 1;
	vk_renderpass_info.pAttachments = &attachment_info;
	vk_renderpass_info.subpassCount = 1;
	vk_renderpass_info.pSubpasses = &subpass_info;
	vk_renderpass_info.dependencyCount = 0;
	vk_renderpass_info.pDependencies = nullptr;


	VkResult result = vkCreateRenderPass(vk_device, &vk_renderpass_info, nullptr, &vk_render_pass);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Unable to create renderpass");
	}
}