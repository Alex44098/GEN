#include "Renderer/FrustumCulling/BoundingCalculation.h"
#include "Renderer/Vulkan/Pipelines/MeshPipeline.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void MeshPipeline::Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) {
	const VkDevice device = vulkan.GetDevice();
	const VkShaderModule vertexShader = Util::LoadShaderModule("shaders/mesh_vert.spv", device);
	Debug::AddDebugLabel4ShaderModule(device, vertexShader, "mesh.vert");

	const VkShaderModule fragmentShader = Util::LoadShaderModule("shaders/mesh_frag.spv", device);
	Debug::AddDebugLabel4ShaderModule(device, fragmentShader, "mesh.frag");

	const VkPushConstantRange bufferRange{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(Constants)
	};

	const VkPushConstantRange pushConstantRanges[1] { bufferRange };
	const VkDescriptorSetLayout layouts[1] { vulkan.GetImageManager().GetDescSetLayout()};
	this->CreatePipelineLayout(device, layouts, pushConstantRanges);

	// Creating shaders and pipeline
	this->SetShaders(vertexShader, fragmentShader);
	this->SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	this->SetPolygonMode(VK_POLYGON_MODE_FILL);
	this->EnableCulling();
	this->SetMultisampling(samples, use_a2c);
	if (use_a2c)
		this->DisableBlending();
	else
		this->EnableBlendingAlpha();
	this->SetColorAttachmentFormat(drawImageFormat);
	this->SetDepthFormat(depthImageFormat);
	this->EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL);
	
	this->BuildPipeline(device);

	Debug::AddDebugLabel4Pipeline(device, this->pipeline, "Mesh pipeline");

	vkDestroyShaderModule(device, vertexShader, nullptr);
	vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void MeshPipeline::SetA2C(bool use_a2c) {
	this->use_a2c = use_a2c;
}

void MeshPipeline::Cleanup(gvk::Vulkan& vulkan) {
	const VkDevice device = vulkan.GetDevice();

	vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
	vkDestroyPipeline(device, this->pipeline, nullptr);
}

void MeshPipeline::Draw(
	VkCommandBuffer cmdBuffer,
	VkExtent2D extent,
	const gvk::Vulkan& vulkan,
	const MeshManager& meshManager,
	const MaterialManager& materialManager,
	const Camera& camera,
	const Buffer& sceneDataBuffer,
	const std::vector<GeometryRenderingUnit>& units,
	const std::vector<std::size_t>& unitsOrder) {

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

	// viewport flipping
	const VkViewport viewport{
		.x = 0,
		.y = 0,
		//.y = static_cast<float>(extent.height),
		.width = static_cast<float>(extent.width),
		.height = static_cast<float>(extent.height),
		//.height = -static_cast<float>(extent.height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

	const VkRect2D scissor{
		.offset = {},
		.extent = extent
	};
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	MeshId prevMeshId = INVALID_MESH_ID;
	const auto cameraFrustum = FrustumCulling::CreateFrustumFromCamera(camera);

	for (const std::size_t curMeshId : unitsOrder) {
		const GeometryRenderingUnit& unit = units[curMeshId];
		if (!FrustumCulling::IsInFrustum(cameraFrustum, unit.worldBoundingSphere))
		{
			// Not working
			// continue;
		}

		const Mesh& mesh = meshManager.GetMesh(unit.meshId);;

		// Loading a new indices
		if (unit.meshId != prevMeshId) {
			vkCmdBindIndexBuffer(cmdBuffer, mesh.indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
			prevMeshId = unit.meshId;
		}

		const Constants pushConstants{
			.transform = unit.transformMatrix,
			.sceneDataBuffer = sceneDataBuffer.address,
			.vertexBuffer = mesh.vertexBuffer.address,
			.materialId = unit.materialId
		};
		vkCmdPushConstants(
			cmdBuffer,
			this->pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
			0,
			sizeof(Constants),
			&pushConstants);

		vkCmdDrawIndexed(cmdBuffer, mesh.indices.size(), 1, 0, 0, 0);
	}
}