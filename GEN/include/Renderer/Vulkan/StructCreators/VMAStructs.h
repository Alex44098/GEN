#pragma once

#include "Renderer/Vulkan/VmaUsage.h"

namespace StructCreators {
	//const VmaVulkanFunctions VmaVkFunctions();
	const VmaAllocatorCreateInfo VmaAllocatorInfo(
		const VkInstance instance,
		const VkPhysicalDevice phDevice,
		const VkDevice logDevice,
		const VmaVulkanFunctions& vmaVulkanFunctions);
}