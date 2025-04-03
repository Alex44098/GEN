#include "Renderer/Vulkan/Util/DebugLabels.h"

namespace Debug {
	void AddDebugLabel4Buffer(VkDevice device, VkBuffer buffer, const char* label) {
		const VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_BUFFER,
			.objectHandle = reinterpret_cast<GECS::u64>(buffer),
			.pObjectName = label
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
	}

	void AddDebugLabel4Image(VkDevice device, VkImage image, const char* label) {
		const VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_IMAGE,
			.objectHandle = reinterpret_cast<GECS::u64>(image),
			.pObjectName = label
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
	}

	void AddDebugLabel4ImageView(VkDevice device, VkImageView imageView, const char* label) {
		const VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
			.objectHandle = reinterpret_cast<GECS::u64>(imageView),
			.pObjectName = label
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
	}

	void AddDebugLabel4Pipeline(VkDevice device, VkPipeline pipeline, const char* label) {
		const VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_PIPELINE,
			.objectHandle = reinterpret_cast<GECS::u64>(pipeline),
			.pObjectName = label
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
	}

	void AddDebugLabel4ShaderModule(VkDevice device, VkShaderModule shaderModule, const char* label) {
		const VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_SHADER_MODULE,
			.objectHandle = reinterpret_cast<GECS::u64>(shaderModule),
			.pObjectName = label
		};
		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
	}

	void BeginDebugLabel(VkCommandBuffer cmdBuffer, const char* label) {
		const VkDebugUtilsLabelEXT cmdLabel{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
			.pLabelName = label,
			.color = {0.f, 0.f, 0.f, 1.f}
		};

		vkCmdBeginDebugUtilsLabelEXT(cmdBuffer, &cmdLabel);
	}

	void EndBeginLabel(VkCommandBuffer cmdBuffer) {
		vkCmdEndDebugUtilsLabelEXT(cmdBuffer);
	}
}