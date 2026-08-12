#include "Renderer/Vulkan/StructCreators/VkBootstrapStructs.h"

namespace StructCreators {
	const VkPhysicalDeviceFeatures PhysicalDeviceFeatures() {
		const VkPhysicalDeviceFeatures physDeviceFeatures {
			.imageCubeArray = VK_TRUE,
			.depthClamp = VK_TRUE,
			.fillModeNonSolid = VK_TRUE,
			.samplerAnisotropy = VK_TRUE,
		};

		return physDeviceFeatures;
	}

	const VkPhysicalDeviceVulkan12Features Vulkan12Features() {
		const VkPhysicalDeviceVulkan12Features vulkanFeatures12{
			.descriptorIndexing = VK_TRUE,
			.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
			.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
			.descriptorBindingPartiallyBound = VK_TRUE,
			.descriptorBindingVariableDescriptorCount = VK_TRUE,
			.runtimeDescriptorArray = VK_TRUE,
			.scalarBlockLayout = VK_TRUE,
			.bufferDeviceAddress = VK_TRUE
		};
		
		return vulkanFeatures12;
	}

	const VkPhysicalDeviceVulkan13Features Vulkan13Features() {
		const VkPhysicalDeviceVulkan13Features vulkanFeatures13{
			.synchronization2 = VK_TRUE, // for VkSubmitInfo2
			.dynamicRendering = VK_TRUE // rendering without RenderPasses
		};
		
		return vulkanFeatures13;
	}
}