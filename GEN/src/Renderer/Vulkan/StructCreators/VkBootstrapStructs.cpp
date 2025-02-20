#include "Renderer/Vulkan/StructCreators/VkBootstrapStructs.h"

namespace StructCreators {
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures() {
		VkPhysicalDeviceFeatures physDeviceFeatures;
		physDeviceFeatures.imageCubeArray = VK_TRUE;
		physDeviceFeatures.depthClamp = VK_TRUE;

		return physDeviceFeatures;
	}

	VkPhysicalDeviceVulkan12Features Vulkan12Features() {
		VkPhysicalDeviceVulkan12Features vulkanFeatures12;
		vulkanFeatures12.descriptorIndexing = VK_TRUE;
		vulkanFeatures12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
		vulkanFeatures12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
		vulkanFeatures12.descriptorBindingPartiallyBound = VK_TRUE;
		vulkanFeatures12.descriptorBindingVariableDescriptorCount = VK_TRUE;
		vulkanFeatures12.runtimeDescriptorArray = VK_TRUE;
		vulkanFeatures12.scalarBlockLayout = VK_TRUE;
		vulkanFeatures12.bufferDeviceAddress = VK_TRUE;

		return vulkanFeatures12;
	}

	VkPhysicalDeviceVulkan13Features Vulkan13Features() {
		VkPhysicalDeviceVulkan13Features vulkanFeatures13;
		vulkanFeatures13.synchronization2 = VK_TRUE; // for VkSubmitInfo2
		vulkanFeatures13.dynamicRendering = VK_TRUE; // rendering without RenderPasses

		return vulkanFeatures13;
	}
}