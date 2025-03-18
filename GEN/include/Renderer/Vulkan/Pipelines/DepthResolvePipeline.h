#pragma once

#include "Renderer/Vulkan/Pipelines/Pipeline.h"
#include "Renderer/Vulkan/Util/ShaderModuleLoader.h"

class DepthResolvePipeline : public Pipeline {
	struct Constants {
		glm::vec2 depthImageSize;
		std::uint32_t depthImageId;
		std::int32_t samples;
	};
public:
	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) override;
	virtual void Cleanup(VkDevice device) override;

	void Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Image& depthImage, int samples);
};