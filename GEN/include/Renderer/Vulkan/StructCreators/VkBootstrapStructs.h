#pragma once

#include <VkBootstrap/VkBootstrap.h>

namespace StructCreators {
	const VkPhysicalDeviceFeatures PhysicalDeviceFeatures();
	const VkPhysicalDeviceVulkan12Features Vulkan12Features();
	const VkPhysicalDeviceVulkan13Features Vulkan13Features();
}
