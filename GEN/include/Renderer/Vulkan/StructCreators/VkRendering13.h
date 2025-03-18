#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>

struct RenderInfoParams {
	VkExtent2D extent;
	VkImageView colorImageView{ VK_NULL_HANDLE };
	bool hasColorClearValue{ false };
	glm::vec4 colorImageClearValue;
	VkImageView depthImageView{ VK_NULL_HANDLE };
	float depthImageClearValue;
	VkImageView resolveImageView{ VK_NULL_HANDLE };
};

struct RenderInfo {
	VkRenderingInfo renderingInfo;
	VkRenderingAttachmentInfo colorAttach;
	VkRenderingAttachmentInfo depthAttach;
};

namespace StructCreators {
	RenderInfo CreateRenderingInfo(const RenderInfoParams& params);
}