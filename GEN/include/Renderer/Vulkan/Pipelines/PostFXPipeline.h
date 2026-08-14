#pragma once

#include "Renderer/Vulkan/Pipelines/Pipeline.h"
#include "Renderer/Vulkan/Util/ShaderModuleLoader.h"

class PostFXPipeline : public Pipeline {
	struct Constants {
		VkDeviceAddress sceneDataBuffer;
		std::uint32_t drawImageId;
		std::uint32_t depthImageId;
	};

public:
	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) override;
	virtual void Cleanup(gvk::Vulkan& vulkan) override;

	void Draw(
		VkCommandBuffer cmd,
		gvk::Vulkan& vulkan,
		const Image& drawImage,
		const Image& depthImage,
		const Buffer& sceneDataBuffer);
};