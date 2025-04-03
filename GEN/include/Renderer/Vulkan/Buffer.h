#pragma once

#include <Volk/volk.h>
#include "Renderer/Vulkan/VmaUsage.h"

struct Buffer {
	VkBuffer vkBuffer;
	VmaAllocation allocation;
	VmaAllocationInfo allocInfo;
	VkDeviceAddress address;
};