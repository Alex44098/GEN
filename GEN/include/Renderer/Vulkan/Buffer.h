#pragma once

#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>

struct Buffer {
	VkBuffer vkBuffer;
	VmaAllocation allocation;
	VmaAllocationInfo allocInfo;
	VkDeviceAddress address;
};