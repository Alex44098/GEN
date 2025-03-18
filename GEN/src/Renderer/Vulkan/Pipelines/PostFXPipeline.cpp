#include "Renderer/Vulkan/Pipelines/PostFXPipeline.h"

void PostFXPipeline::Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) {
	const VkDevice device = vulkan.GetDevice();
	const VkShaderModule vertexShader = Util::LoadShaderModule("shaders/skybox_vert.spv", device);
	const VkShaderModule fragmentShader = Util::LoadShaderModule("shaders/postFX_frag.spv", device);

	const VkPushConstantRange bufferRange{
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(Constants)
	};
	const VkPushConstantRange pushConstantRanges[1]{ bufferRange };
	const VkDescriptorSetLayout layouts[1]{ vulkan.GetImageManager().GetDescSetLayout() };
	this->CreatePipelineLayout(device, layouts, pushConstantRanges);

	// Creating shaders and pipeline
	this->SetShaders(vertexShader, fragmentShader);
	this->SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	this->SetPolygonMode(VK_POLYGON_MODE_FILL);
	this->DisableCulling();
	this->SetMultisamplingEmpty();
	this->DisableBlending();
	this->SetColorAttachmentFormat(drawImageFormat);
	this->DisableDepthTest();

	this->BuildPipeline(device);

	vkDestroyShaderModule(device, vertexShader, nullptr);
	vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void PostFXPipeline::Cleanup(VkDevice device) {
	vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(device, this->pipeline, nullptr);
}

void PostFXPipeline::Draw(
	VkCommandBuffer cmdBuffer,
	gvk::Vulkan& vulkan,
	const Image& drawImage,
	const Image& depthImage,
	const Buffer& sceneDataBuffer) {

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

	// Binding bindless desc set
	vkCmdBindDescriptorSets(
		cmdBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipelineLayout,
		0,
		1,
		&vulkan.GetImageManager().GetDescSet(),
		0,
		nullptr);

	const Constants pushConstants{
		.sceneDataBuffer = sceneDataBuffer.address,
		.drawImageId = drawImage.id,
		.depthImageId = depthImage.id
	};
	vkCmdPushConstants(cmdBuffer, this->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Constants), &pushConstants);

	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
}