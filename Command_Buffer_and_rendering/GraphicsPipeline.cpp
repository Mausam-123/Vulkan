#include <fstream>
#include "Vulkan.h"

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule vk_simple_triangle::create_shader_module(const std::vector<char>& code) {
    VkShaderModule shader_handle;
    VkShaderModuleCreateInfo shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.pNext = nullptr;
    shader_info.flags = 0;
    shader_info.codeSize = code.size();
    shader_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkResult result = vkCreateShaderModule(vk_device, &shader_info, nullptr, &shader_handle);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Unable to create shader Module");
    }

    return shader_handle;
}

void vk_simple_triangle::vk_create_pipeline() {
    VkResult result = VK_ERROR_UNKNOWN;

    auto vertex_shader_code = readFile("Shaders/vert.spv");
    auto frag_shader_code = readFile("Shaders/frag.spv");

    vertex_shader_module = create_shader_module(vertex_shader_code);
    frag_shader_module = create_shader_module(frag_shader_code);

    //Vertex Shader Information
    VkPipelineShaderStageCreateInfo vertexStageInfo = {};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.pNext = nullptr;
    vertexStageInfo.module = vertex_shader_module;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.flags = 0;
    vertexStageInfo.pName = "main";
    vertexStageInfo.pSpecializationInfo = nullptr;
    
    //Fragment Shader Information
    VkPipelineShaderStageCreateInfo fragStageInfo = {};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.pNext = nullptr;
    fragStageInfo.module = frag_shader_module;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.flags = 0;
    fragStageInfo.pName = "main";
    fragStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexStageInfo, fragStageInfo };

    //Set ViewPort and Scissor as dynmaic
    std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.pNext = nullptr;
    dynamicStateInfo.flags = 0;
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamicStateInfo.pDynamicStates = dynamic_states.data();

    VkViewport viewPort = {};
    viewPort.width = static_cast<float>(swapchain_extent.width);
    viewPort.height = static_cast<float>(swapchain_extent.height);
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain_extent;

    VkPipelineViewportStateCreateInfo viewPortInfo = {};
    viewPortInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewPortInfo.pNext = nullptr;
    viewPortInfo.viewportCount = 1;
    viewPortInfo.pViewports = nullptr; //Mentioned as dynamic
    viewPortInfo.scissorCount = 1;
    viewPortInfo.pScissors = nullptr; //Mentioned as dynamic

    //Vertex Input Stage - Filling binding and attribute that will be passed to vertex shader
    VkPipelineVertexInputStateCreateInfo vertexInputStage = {};
    vertexInputStage.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStage.pNext = nullptr;
    vertexInputStage.vertexAttributeDescriptionCount = 0;
    vertexInputStage.pVertexAttributeDescriptions = nullptr;
    vertexInputStage.vertexAttributeDescriptionCount = 0;
    vertexInputStage.pVertexAttributeDescriptions = nullptr;

    //Input Assembly Stage - How to draw geometry on screen
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStage = {};
    inputAssemblyStage.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStage.pNext = nullptr;
    inputAssemblyStage.primitiveRestartEnable = VK_FALSE;
    inputAssemblyStage.flags = 0;
    inputAssemblyStage.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    //Rasterizer - Primitives to Fragement conversion
    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.pNext = nullptr;
    rasterizationInfo.flags = 0;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.depthBiasEnable = VK_FALSE;
    rasterizationInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationInfo.depthBiasClamp = VK_FALSE;
    rasterizationInfo.depthBiasConstantFactor = 0.0f;

    //MultiSampling  -- Currently disabled 
    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.pNext = nullptr;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.minSampleShading = 1.0f;
    multisamplingInfo.pSampleMask = nullptr;
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingInfo.alphaToOneEnable = VK_FALSE;

    //Blending Attatchment
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | 
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorblendInfo = {};
    colorblendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorblendInfo.pNext = nullptr;
    colorblendInfo.logicOpEnable = VK_FALSE;
    colorblendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorblendInfo.attachmentCount = 1;
    colorblendInfo.pAttachments = &colorBlendAttachment;
    colorblendInfo.blendConstants[0] = 0.0f;
    colorblendInfo.blendConstants[1] = 0.0f;
    colorblendInfo.blendConstants[2] = 0.0f;
    colorblendInfo.blendConstants[3] = 0.0f;

    //PipelineLayout - Unifrom Buffer data
    VkPipelineLayoutCreateInfo pipelinelayoutInfo = {};
    pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelinelayoutInfo.pNext = nullptr;
    pipelinelayoutInfo.setLayoutCount = 0;
    pipelinelayoutInfo.pSetLayouts = nullptr;
    pipelinelayoutInfo.flags = 0;
    pipelinelayoutInfo.pushConstantRangeCount = 0;
    pipelinelayoutInfo.pPushConstantRanges = nullptr;
    
    result = vkCreatePipelineLayout(vk_device, &pipelinelayoutInfo, nullptr, &vk_pipelineLayout);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Unable to create pipelineLayout");
    }

    VkGraphicsPipelineCreateInfo pipeline_info = { };
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = nullptr;
    pipeline_info.stageCount = 2;       //Vertex Shader and Fragment Shader stages
    pipeline_info.pStages = shaderStages;
    pipeline_info.pVertexInputState = &vertexInputStage;
    pipeline_info.pInputAssemblyState = &inputAssemblyStage;
    pipeline_info.pTessellationState = nullptr;
    pipeline_info.pViewportState = &viewPortInfo;
    pipeline_info.pRasterizationState = &rasterizationInfo;
    pipeline_info.pMultisampleState = &multisamplingInfo;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &colorblendInfo;
    pipeline_info.pDynamicState = &dynamicStateInfo;
    pipeline_info.layout = vk_pipelineLayout;
    pipeline_info.renderPass = vk_render_pass;
    pipeline_info.subpass = 0;          // Consider subpass at index 0
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;       //Invalid Index

    result = vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vk_graphicsPipeline);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Unable to create Graphics pipeline");
    }

}