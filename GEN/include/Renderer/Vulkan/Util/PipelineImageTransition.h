#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Vulkan/StructCreators/VkImageSubresourceRange.h"

namespace Util {
	void PipelineImageTransition(VkCommandBuffer vkBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
}