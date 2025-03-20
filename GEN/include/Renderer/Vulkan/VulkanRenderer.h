#pragma once

#include <Volk/volk.h>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"
#include "Renderer/Vulkan/Image.h"
#include "Renderer/Gltf/GLTFScene.h"
#include "Renderer/Gltf/GLTFLight.h"
#include "Renderer/Vulkan/PreallocatedBuffer.h"
#include "Renderer/Vulkan/GeometryRenderingUnit.h"

#include "Renderer/Vulkan/Pipelines/MeshPipeline.h"
#include "Renderer/Vulkan/Pipelines/SkyboxPipeline.h"
#include "Renderer/Vulkan/Pipelines/DepthResolvePipeline.h"
#include "Renderer/Vulkan/Pipelines/PostFXPipeline.h"

#include "Renderer/Vulkan/StructCreators/VkRendering13.h"
#include "Renderer/Vulkan/StructCreators/VkImageSubresourceRange.h"
#include "Renderer/Vulkan/Util/PipelineImageTransition.h"

class VulkanRenderer {
	MeshManager& meshManager;
	MaterialManager& materialManager;

	// scene data
	PreallocatedBuffer sceneDataBuffer;

	// mesh and material rendering order
	std::vector<GeometryRenderingUnit> renderingUnits;
	std::vector<std::size_t> renderingUnitsOrder;

	// image and depth images
	ImageId drawImageId{ INVALID_IMAGE_ID };
	ImageId resolveDrawImageId{ INVALID_IMAGE_ID };
	ImageId depthImageId{ INVALID_IMAGE_ID };
	ImageId resolveDepthImageId{ INVALID_IMAGE_ID };
	ImageId postFXDrawImageId{ INVALID_IMAGE_ID };

	bool resolveImagesInitialized{ false };

	// pipelines
	MeshPipeline meshPipeline;
	SkyboxPipeline skyboxPipeline;
	DepthResolvePipeline depthResolvePipeline;
	PostFXPipeline postFXPipeline;

	VkFormat drawImageFormat{ VK_FORMAT_R16G16B16A16_SFLOAT };
	VkFormat depthImageFormat{ VK_FORMAT_D32_SFLOAT };
	VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };

	// lights data
	PreallocatedBuffer lightDataBuffer;
	std::vector<Gltf::GLTFLightData> lightData;
	GECS::i64 sunlightIndex{ -1 };

public:
	VulkanRenderer(MeshManager& meshManager, MaterialManager& materialManager);

	void Init(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize);
	void Destroy(gvk::Vulkan& vulkan);

	void RenderFrame(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData);

	void StartFrameBuilding();
	void AddLight(const Gltf::GLTFLight& light, const Transform& transform);
	void AddRenderingUnit(MeshId meshId, MaterialId materialId, const glm::mat4& transform, bool castShadow);
	void EndFrameBuilding();

	inline ImageId GetDrawImageId() const { return this->drawImageId; }

	const Image& GetDrawImage(gvk::Vulkan& vulkan) const;
	const Image& GetDepthImage(gvk::Vulkan& vulkan) const;
	VkFormat GetDrawImageFormat() const;
	VkFormat GetDepthImageFormat() const;

private:
	void InitSceneData(gvk::Vulkan& vulkan);
	void CreateImages(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize);
	void SortRenderingUnits();
	int SamplesToInt(VkSampleCountFlagBits samples);

	bool MultisamplingEnabled() const;
};