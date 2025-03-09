#include "Renderer/Vulkan/StructCreators/VkImageSubresourceRange.h"

namespace StructCreators {
	VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectFlags) {
		VkImageSubresourceRange range{
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS
		};

		return range;
	}
}