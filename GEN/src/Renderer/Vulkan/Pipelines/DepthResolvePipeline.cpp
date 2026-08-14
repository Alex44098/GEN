#include "Renderer/Vulkan/Pipelines/DepthResolvePipeline.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void DepthResolvePipeline::Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) {
	const VkDevice device = vulkan.GetDevice();
	const VkShaderModule vertexShader = Util::LoadShaderModule("shaders/fullscreen_vert.spv", device);
	const VkShaderModule fragmentShader = Util::LoadShaderModule("shaders/depthResolve_frag.spv", device);

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
	this->SetDepthFormat(depthImageFormat);
	this->EnableDepthTest(true, VK_COMPARE_OP_ALWAYS);

	this->BuildPipeline(device);

	Debug::AddDebugLabel4Pipeline(device, this->pipeline, "Depth resolve pipeline");

	vkDestroyShaderModule(device, vertexShader, nullptr);
	vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void DepthResolvePipeline::Cleanup(gvk::Vulkan& vulkan) {
	const VkDevice device = vulkan.GetDevice();

	vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(device, this->pipeline, nullptr);
}

void DepthResolvePipeline::Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Image& depthImage, int samples) {
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
		.depthImageSize = {
			(float)depthImage.getExtent2D().width,
			(float)depthImage.getExtent2D().height
		},
		.depthImageId = depthImage.id,
		.samples = samples
	};
	vkCmdPushConstants(cmdBuffer, this->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Constants), &pushConstants);

	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
}