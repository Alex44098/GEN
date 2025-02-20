#pragma once

#include <VkBootstrap/VkBootstrap.h>

namespace StructCreators {
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures();
	VkPhysicalDeviceVulkan12Features Vulkan12Features();
	VkPhysicalDeviceVulkan13Features Vulkan13Features();
}
