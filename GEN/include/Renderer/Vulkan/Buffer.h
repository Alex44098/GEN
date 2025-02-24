#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

struct Buffer {
	VkBuffer vkBuffer;
	VmaAllocation allocation;
	VmaAllocationInfo allocInfo;
	VkDeviceAddress address;
};