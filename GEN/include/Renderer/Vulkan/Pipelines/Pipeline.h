#pragma once

#include <Volk/volk.h>
#include <span>

#include "Renderer/Vulkan/Vulkan.h"

class Pipeline {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineRenderingCreateInfo renderInfo;
    VkFormat colorAttachmentformat;
    bool dynamicDepth{ false };
	
protected:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

public:
    Pipeline();

	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) = 0;
	virtual void Cleanup(VkDevice device) = 0;

protected:
	void CreatePipelineLayout(VkDevice device, std::span<const VkDescriptorSetLayout> layouts,
		std::span<const VkPushConstantRange> pushConstantRanges);
    void BuildPipeline(VkDevice device);
    void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
    void SetShaders(VkShaderModule vertexShader, VkShaderModule geometryShader, VkShaderModule fragmentShader);
    void SetInputTopology(VkPrimitiveTopology topology);
    void SetPolygonMode(VkPolygonMode mode);
    void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    void EnableCulling();
    void DisableCulling();
    void SetMultisamplingEmpty();
    void SetMultisampling(VkSampleCountFlagBits samples);
    void DisableBlending();
    void EnableBlending(
        VkBlendOp blendOp = VK_BLEND_OP_ADD,
        VkBlendFactor srcBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        VkBlendFactor dstBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
    void SetColorAttachmentFormat(VkFormat format);
    void SetDepthFormat(VkFormat format);
    void EnableDepthTest(bool depthWriteEnable, VkCompareOp op);
    void EnableDepthClamp();
    void DisableDepthTest();
    void EnableDynamicDepth();
    void EnableDepthBias(float constantFactor, float slopeFactor);
};