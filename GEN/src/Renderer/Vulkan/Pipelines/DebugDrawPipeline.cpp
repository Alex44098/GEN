#include <algorithm>

#include "Renderer/FrustumCulling/BoundingCalculation.h"
#include "Renderer/Vulkan/Pipelines/DebugDrawPipeline.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void DebugDrawPipeline::Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) {
	const VkDevice device = vulkan.GetDevice();
	const VkShaderModule vertexShader = Util::LoadShaderModule("shaders/debugLines_vert.spv", device);
	Debug::AddDebugLabel4ShaderModule(device, vertexShader, "debugLines.vert");

	const VkShaderModule fragmentShader = Util::LoadShaderModule("shaders/debugLines_frag.spv", device);
	Debug::AddDebugLabel4ShaderModule(device, fragmentShader, "debugLines.frag");

	const VkPushConstantRange bufferRange{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(Constants)
	};

	const VkPushConstantRange pushConstantRanges[1]{ bufferRange };
	const VkDescriptorSetLayout layouts[1]{ vulkan.GetImageManager().GetDescSetLayout() };
	this->CreatePipelineLayout(device, layouts, pushConstantRanges);

	// Creating shaders and pipeline
	this->SetShaders(vertexShader, fragmentShader);
	this->SetInputTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
	this->SetPolygonMode(VK_POLYGON_MODE_LINE);
	this->DisableCulling();
	this->SetMultisampling(samples, false);
	this->EnableBlendingAlpha();
	this->SetColorAttachmentFormat(drawImageFormat);
	this->SetDepthFormat(depthImageFormat);
	this->EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

	this->BuildPipeline(device);

	Debug::AddDebugLabel4Pipeline(device, this->pipeline, "Debug draw pipeline");

	vkDestroyShaderModule(device, vertexShader, nullptr);
	vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void DebugDrawPipeline::Cleanup(gvk::Vulkan& vulkan) {
	const VkDevice device = vulkan.GetDevice();

	vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(device, this->pipeline, nullptr);

	if (debugVertexBuffer.vkBuffer != VK_NULL_HANDLE && currentBufferSize > 0) {
		vulkan.DestroyBuffer(debugVertexBuffer);
	}
}

void DebugDrawPipeline::AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color) {
	lineVertices.push_back({ from, color });
	lineVertices.push_back({ to, color });
}

void DebugDrawPipeline::AddFrustum(const FrustumCulling::Frustum& frustum, const glm::vec3& color) {

}

void DebugDrawPipeline::AddSphere(const Primitives::Sphere& sphere, const glm::vec3& color, int rings, int sectors) {
	const float r = sphere.radius;
	const glm::vec3 c = sphere.center;
	// Рисуем три ортогональные окружности
	const int steps = sectors;
	for (int axis = 0; axis < 3; ++axis) {
		glm::vec3 u, v;
		if (axis == 0) { u = glm::vec3(0, 1, 0); v = glm::vec3(0, 0, 1); }
		else if (axis == 1) { u = glm::vec3(1, 0, 0); v = glm::vec3(0, 0, 1); }
		else { u = glm::vec3(1, 0, 0); v = glm::vec3(0, 1, 0); }
		glm::vec3 prevPoint = c + u * r;
		for (int i = 1; i <= steps; ++i) {
			float angle = glm::two_pi<float>() * i / steps;
			glm::vec3 p = c + (u * cos(angle) + v * sin(angle)) * r;
			AddLine(prevPoint, p, color);
			prevPoint = p;
		}
	}
}

void DebugDrawPipeline::AddAABB(const Primitives::AABB& aabb, const glm::vec3& color) {

}

void DebugDrawPipeline::Prepare(gvk::Vulkan& vulkan) {
	if (lineVertices.empty()) {
		return;
	}

	VkDeviceSize requiredSize = lineVertices.size() * sizeof(DebugDrawVertex);

	if (currentBufferSize < requiredSize) {
		if (debugVertexBuffer.vkBuffer != VK_NULL_HANDLE && currentBufferSize > 0) {
			vulkan.DestroyBuffer(debugVertexBuffer);
		}

		currentBufferSize = std::max(requiredSize, currentBufferSize * 2);
		
		if (currentBufferSize < 65536)
			currentBufferSize = 65536; // 64 KiB

		debugVertexBuffer = vulkan.CreateBuffer(
			currentBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO);
	}

	void* mapped = debugVertexBuffer.allocInfo.pMappedData;
	memcpy(mapped, lineVertices.data(), requiredSize);
}

void DebugDrawPipeline::Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Camera& camera) {
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
			.viewProj = camera.GetViewProjection(),
			.vertexBufferAddress = debugVertexBuffer.address
	};

	vkCmdPushConstants(
		cmdBuffer,
		this->pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(Constants),
		&pushConstants);

	vkCmdDraw(cmdBuffer, static_cast<uint32_t>(lineVertices.size()), 1, 0, 0);
}

void DebugDrawPipeline::Clear() {
	lineVertices.clear();
}