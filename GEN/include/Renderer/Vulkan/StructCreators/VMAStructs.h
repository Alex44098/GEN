#pragma once

#include <vma/vk_mem_alloc.h>

namespace StructCreators {
	VmaVulkanFunctions VmaVkFunctions();
	VmaAllocatorCreateInfo VmaAllocatorInfo(
		const vkb::Instance instance,
		const vkb::PhysicalDevice phDevice,
		const vkb::Device logDevice,
		VmaVulkanFunctions* vmaVulkanFunctions);
}