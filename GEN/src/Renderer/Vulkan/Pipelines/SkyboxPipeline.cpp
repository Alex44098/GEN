#include "Renderer/Vulkan/Pipelines/SkyboxPipeline.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void SkyboxPipeline::Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) {
	const VkDevice device = vulkan.GetDevice();
	const VkShaderModule vertexShader = Util::LoadShaderModule("shaders/fullscreen_vert.spv", device);
	const VkShaderModule fragmentShader = Util::LoadShaderModule("shaders/skybox_frag.spv", device);

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
	this->SetMultisampling(samples, false);
	this->DisableBlending();
	this->SetColorAttachmentFormat(drawImageFormat);
	this->SetDepthFormat(depthImageFormat);
	this->EnableDepthTest(false, VK_COMPARE_OP_EQUAL);

	this->BuildPipeline(device);

	Debug::AddDebugLabel4Pipeline(device, this->pipeline, "Skybox pipeline");

	vkDestroyShaderModule(device, vertexShader, nullptr);
	vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void SkyboxPipeline::Cleanup(VkDevice device) {
	vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(device, this->pipeline, nullptr);
}

void SkyboxPipeline::SetSkyboxTexture(const ImageId skyboxId) {
	this->skyboxTextureId = skyboxId;
}

void SkyboxPipeline::Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Camera& camera) {
	if (skyboxTextureId == INVALID_IMAGE_ID)
		return;

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
		.invViewProj = glm::inverse(camera.GetViewProjection()),
		.cameraPos = glm::vec4{camera.GetPosition(), 1.f},
		.skyboxTextureId = (std::uint32_t)this->skyboxTextureId
	};
	vkCmdPushConstants(
		cmdBuffer,
		this->pipelineLayout,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		sizeof(Constants),
		&pushConstants);

	vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
}