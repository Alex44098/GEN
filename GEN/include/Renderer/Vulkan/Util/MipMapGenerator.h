#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Util/PipelineImageTransition.h"

namespace Util {
	void GenerateMipMaps(VkCommandBuffer cmdBuffer, VkImage image, VkExtent2D imageSize, GECS::u32 mipLevels);
}