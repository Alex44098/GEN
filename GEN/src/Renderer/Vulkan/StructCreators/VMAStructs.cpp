#include "Renderer/Vulkan/StructCreators/VMAStructs.h"

#include "Renderer/Vulkan/VmaUsage.h"

namespace StructCreators {
	// Doesn't work, because it needs funcs from volk.h

	/*const VmaVulkanFunctions VmaVkFunctions() {
		const VmaVulkanFunctions vulkanFunctions{
			.vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = &vkGetDeviceProcAddr
		};

		return vulkanFunctions;
	}*/

	const VmaAllocatorCreateInfo VmaAllocatorInfo(
		const VkInstance instance,
		const VkPhysicalDevice phDevice,
		const VkDevice logDevice,
		const VmaVulkanFunctions& vmaVulkanFunctions) {

		const VmaAllocatorCreateInfo vmaAllocatorInfo{
			.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
			.physicalDevice = phDevice,
			.device = logDevice,
			.pVulkanFunctions = &vmaVulkanFunctions,
			.instance = instance
		};

		return vmaAllocatorInfo;
	}
}