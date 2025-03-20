#pragma once

#include <fstream>
#include <vector>
#include <Volk/volk.h>

namespace Util {
	VkShaderModule LoadShaderModule(const char* filePath, VkDevice device);
}