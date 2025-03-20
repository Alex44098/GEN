#pragma once

#include <Volk/volk.h>

namespace StructCreators {
	const VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
    const VkSubmitInfo2 SubmitInfo(
        const VkCommandBufferSubmitInfo* cmd,
        const VkSemaphoreSubmitInfo* waitSemaphoreInfo,
        const VkSemaphoreSubmitInfo* signalSemaphoreInfo);
}