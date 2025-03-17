#pragma once

#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

namespace Util {
	VkShaderModule LoadShaderModule(const char* filePath, VkDevice device);
}