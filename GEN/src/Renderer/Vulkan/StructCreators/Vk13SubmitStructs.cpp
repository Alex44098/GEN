#include "Renderer/Vulkan/StructCreators/Vk13SubmitStructs.h"

namespace StructCreators {
	const VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
		const VkSemaphoreSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = semaphore,
			.value = 1,
			.stageMask = stageMask,
			.deviceIndex = 0
		};

		return submitInfo;
	}

	const VkSubmitInfo2 SubmitInfo(
		const VkCommandBufferSubmitInfo* cmd,
		const VkSemaphoreSubmitInfo* waitSemaphoreInfo,
		const VkSemaphoreSubmitInfo* signalSemaphoreInfo) {

		const VkSubmitInfo2 submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = waitSemaphoreInfo ? 1U : 0U,
			.pWaitSemaphoreInfos = waitSemaphoreInfo,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = cmd,
			.signalSemaphoreInfoCount = signalSemaphoreInfo ? 1U : 0U,
			.pSignalSemaphoreInfos = signalSemaphoreInfo
		};

		return submitInfo;
	}
}