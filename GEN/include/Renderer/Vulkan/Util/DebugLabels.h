#pragma once

#include <Volk/volk.h>

#include "Engine/EngineTypes.h"

namespace Debug {
	void AddDebugLabel4Buffer(VkDevice device, VkBuffer buffer, const char* label);
	void AddDebugLabel4Image(VkDevice device, VkImage image, const char* label);
	void AddDebugLabel4ImageView(VkDevice device, VkImageView imageView, const char* label);
	void AddDebugLabel4Pipeline(VkDevice device, VkPipeline pipeline, const char* label);
	void AddDebugLabel4ShaderModule(VkDevice device, VkShaderModule shaderModule, const char* label);

	void BeginDebugLabel(VkCommandBuffer cmdBuffer, const char* label);
	void EndBeginLabel(VkCommandBuffer cmdBuffer);
}