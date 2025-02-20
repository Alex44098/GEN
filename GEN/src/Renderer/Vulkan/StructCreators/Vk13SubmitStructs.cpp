#include "Renderer/Vulkan/StructCreators/Vk13SubmitStructs.h"

namespace StructCreators {
	VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
		VkSemaphoreSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		submitInfo.semaphore = semaphore;
		submitInfo.value = 1;
		submitInfo.stageMask = stageMask;
		submitInfo.deviceIndex = 0;

		return submitInfo;
	}

	VkSubmitInfo2 SubmitInfo(
		const VkCommandBufferSubmitInfo* cmd,
		const VkSemaphoreSubmitInfo* waitSemaphoreInfo,
		const VkSemaphoreSubmitInfo* signalSemaphoreInfo) {

		VkSubmitInfo2 submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfo;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = cmd;
		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfo;

		return submitInfo;
	}
}