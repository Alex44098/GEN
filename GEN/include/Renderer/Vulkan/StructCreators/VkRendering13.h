#pragma once

#include <optional>

#include <Volk/volk.h>
#include <glm.hpp>

struct RenderInfoParams {
	VkExtent2D extent;
	VkImageView colorImageView{ VK_NULL_HANDLE };
	std::optional<glm::vec4> colorImageClearValue;
	VkImageView depthImageView{ VK_NULL_HANDLE };
	std::optional<float> depthImageClearValue;
	VkImageView resolveImageView{ VK_NULL_HANDLE };
};

struct RenderInfo {
	VkRenderingAttachmentInfo colorAttach;
	VkRenderingAttachmentInfo depthAttach;
	VkRenderingInfo renderingInfo;
};

namespace StructCreators {
	RenderInfo CreateRenderingInfo(const RenderInfoParams& params);
}