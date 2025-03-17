#include "Renderer/Vulkan/Pipelines/Pipeline.h"

Pipeline::Pipeline() {
	this->shaderStages.clear();

	this->inputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	this->rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	this->colorBlendAttachment = {};
	this->multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	this->depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	this->renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
}

void Pipeline::CreatePipelineLayout(VkDevice device, std::span<const VkDescriptorSetLayout> layouts,
	std::span<const VkPushConstantRange> pushConstantRanges) {

	const VkPipelineLayoutCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = (std::uint32_t)layouts.size(),
		.pSetLayouts = layouts.data(),
		.pushConstantRangeCount = (std::uint32_t)pushConstantRanges.size(),
		.pPushConstantRanges = pushConstantRanges.data()
	};

	vkCreatePipelineLayout(device, &createInfo, nullptr, &(this->pipelineLayout));
}

void Pipeline::BuildPipeline(VkDevice device) {
    const VkPipelineViewportStateCreateInfo viewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    const VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &(this->colorBlendAttachment)
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    std::vector<VkDynamicState> dynamicStates { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    if (this->dynamicDepth) {
        dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
    }

    const VkPipelineDynamicStateCreateInfo dynamicInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (std::uint32_t)dynamicStates.size(),
        .pDynamicStates = dynamicStates.data(),
    };

    const VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = (std::uint32_t)(this->shaderStages.size()),
        .pStages = this->shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &(this->inputAssembly),
        .pViewportState = &viewportState,
        .pRasterizationState = &(this->rasterizer),
        .pMultisampleState = &(this->multisampling),
        .pDepthStencilState = &(this->depthStencil),
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicInfo,
        .layout = this->pipelineLayout,
    };

    const VkResult res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(this->pipeline));
    assert(res == VK_SUCCESS && "Pipeline didn't created");
}

void Pipeline::SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader) {
    shaderStages.clear();

    if (vertexShader) {
        this->shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            }
        );
    }
    if (fragmentShader) {
        this->shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        );
    }
}

void Pipeline::SetShaders(VkShaderModule vertexShader, VkShaderModule geometryShader, VkShaderModule fragmentShader) {
    shaderStages.clear();

    if (vertexShader) {
        this->shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            }
        );
    }
    if (geometryShader) {
        this->shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
                .module = geometryShader,
                .pName = "main"
            }
        );
    }
    if (fragmentShader) {
        this->shaderStages.push_back(VkPipelineShaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        );
    }
}

void Pipeline::SetInputTopology(VkPrimitiveTopology topology) {
    this->inputAssembly.topology = topology;
    this->inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void Pipeline::SetPolygonMode(VkPolygonMode mode) {
    this->rasterizer.polygonMode = mode;
    this->rasterizer.lineWidth = 1.f;
}

void Pipeline::SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace) {
    this->rasterizer.cullMode = cullMode;
    this->rasterizer.frontFace = frontFace;
}

void Pipeline::EnableCulling() {
    this->SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
}

void Pipeline::DisableCulling() {
    this->SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
}

void Pipeline::SetMultisamplingEmpty() {
    this->multisampling.sampleShadingEnable = VK_FALSE;
    this->multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    this->multisampling.minSampleShading = 1.0f;
    this->multisampling.alphaToCoverageEnable = VK_FALSE;
    this->multisampling.alphaToOneEnable = VK_FALSE;
}

void Pipeline::SetMultisampling(VkSampleCountFlagBits samples) {
    this->multisampling.sampleShadingEnable = VK_FALSE;
    this->multisampling.rasterizationSamples = samples;
    this->multisampling.minSampleShading = 1.0f;
    this->multisampling.alphaToCoverageEnable = VK_FALSE;
    this->multisampling.alphaToOneEnable = VK_FALSE;
}

void Pipeline::DisableBlending() {
    this->colorBlendAttachment.blendEnable = VK_FALSE;
    this->colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
}

void Pipeline::EnableBlending(
    VkBlendOp blendOp,
    VkBlendFactor srcBlendFactor,
    VkBlendFactor dstBlendFactor,
    VkBlendFactor srcAlphaBlendFactor,
    VkBlendFactor dstAlphaBlendFactor) {

    this->colorBlendAttachment.blendEnable = VK_TRUE;
    this->colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    this->colorBlendAttachment.srcColorBlendFactor = srcBlendFactor;
    this->colorBlendAttachment.dstColorBlendFactor = dstBlendFactor;
    this->colorBlendAttachment.colorBlendOp = blendOp;
    this->colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    this->colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    this->colorBlendAttachment.alphaBlendOp = blendOp;
}

void Pipeline::SetColorAttachmentFormat(VkFormat format) {
    this->colorAttachmentformat = format;
    this->renderInfo.colorAttachmentCount = 1;
    this->renderInfo.pColorAttachmentFormats = &(this->colorAttachmentformat);
}

void Pipeline::SetDepthFormat(VkFormat format) {
    this->renderInfo.depthAttachmentFormat = format;
}

void Pipeline::EnableDepthTest(bool depthWriteEnable, VkCompareOp op) {
    this->depthStencil.depthTestEnable = VK_TRUE;
    this->depthStencil.depthWriteEnable = depthWriteEnable;
    this->depthStencil.depthCompareOp = op;
    this->depthStencil.depthBoundsTestEnable = VK_FALSE;
    this->depthStencil.stencilTestEnable = VK_FALSE;
    this->depthStencil.front = {};
    this->depthStencil.back = {};
    this->depthStencil.minDepthBounds = 0.f;
    this->depthStencil.maxDepthBounds = 1.f;
}

void Pipeline::EnableDepthClamp() {
    this->rasterizer.depthClampEnable = true;
}

void Pipeline::DisableDepthTest() {
    this->depthStencil.depthTestEnable = VK_FALSE;
    this->depthStencil.depthWriteEnable = VK_FALSE;
    this->depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    this->depthStencil.depthBoundsTestEnable = VK_FALSE;
    this->depthStencil.stencilTestEnable = VK_FALSE;
    this->depthStencil.front = {};
    this->depthStencil.back = {};
    this->depthStencil.minDepthBounds = 0.f;
    this->depthStencil.maxDepthBounds = 1.f;
}

void Pipeline::EnableDynamicDepth() {
    this->dynamicDepth = true;
}

void Pipeline::EnableDepthBias(float constantFactor, float slopeFactor) {
    this->rasterizer.depthBiasEnable = true;
    this->rasterizer.depthBiasConstantFactor = constantFactor;
    this->rasterizer.depthBiasSlopeFactor = slopeFactor;
}