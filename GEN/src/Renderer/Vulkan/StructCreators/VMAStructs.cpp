#include <VkBootstrap/VkBootstrap.h>
#include "Renderer/Vulkan/StructCreators/VMAStructs.h"

namespace StructCreators {
	VmaVulkanFunctions VmaVkFunctions() {
		VmaVulkanFunctions vulkanFunctions;
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		return vulkanFunctions;
	}

	VmaAllocatorCreateInfo VmaAllocatorInfo(
		const vkb::Instance instance,
		const vkb::PhysicalDevice phDevice,
		const vkb::Device logDevice,
		VmaVulkanFunctions* vmaVulkanFunctions) {

		VmaAllocatorCreateInfo vmaAllocatorInfo;
		vmaAllocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaAllocatorInfo.physicalDevice = phDevice;
		vmaAllocatorInfo.device = logDevice;
		vmaAllocatorInfo.pVulkanFunctions = vmaVulkanFunctions;
		vmaAllocatorInfo.instance = instance;

		return vmaAllocatorInfo;
	}
}