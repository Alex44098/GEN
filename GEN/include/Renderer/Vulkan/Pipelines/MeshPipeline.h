#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/Vulkan/Pipelines/Pipeline.h"

#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"
#include "Renderer/Vulkan/GeometryRenderingUnit.h"
#include "Renderer/Camera.h"

class MeshPipeline : public Pipeline {
	struct UnitParameters {
		glm::mat4 transform;
		VkDeviceAddress sceneDataBuffer;
		VkDeviceAddress vertexBuffer;
		std::uint32_t materialId;
		std::uint32_t padding;
	};

public:
	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) override;
	virtual void Cleanup(VkDevice device) override;

	void Draw(
		VkCommandBuffer cmd,
		VkExtent2D extent,
		const gvk::Vulkan& vulkan,
		const MeshManager& meshCache,
		const MaterialManager& materialCache,
		const Camera& camera,
		const Buffer& sceneDataBuffer,
		const std::vector<GeometryRenderingUnit>& units,
		const std::vector<std::size_t>& unitsOrder);
};