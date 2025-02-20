#pragma once

#include <vulkan/vulkan.h>

namespace StructCreators {
	VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
    VkSubmitInfo2 SubmitInfo(
        const VkCommandBufferSubmitInfo* cmd,
        const VkSemaphoreSubmitInfo* waitSemaphoreInfo,
        const VkSemaphoreSubmitInfo* signalSemaphoreInfo);
}