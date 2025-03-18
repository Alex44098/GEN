#include "Renderer/Vulkan/Swapchain.h"

void Swapchain::Init(VkDevice logDevice) {
	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (GECS::i32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkCreateFence(logDevice, &fenceCreateInfo, nullptr, &(this->frameSyncs.fenceInFlight[i]));
		vkCreateSemaphore(logDevice, &semaphoreCreateInfo, nullptr, &(this->frameSyncs.semaphoreImageAvailable[i]));
		vkCreateSemaphore(logDevice, &semaphoreCreateInfo, nullptr, &(this->frameSyncs.semaphoreRenderFinished[i]));
	}
}

void Swapchain::Create(vkb::Device device, VkFormat format, GECS::u32 width, GECS::u32 height) {
	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.format = format;
	surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

	vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ device }
		.set_desired_format(surfaceFormat)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // vsync
		.set_desired_extent(width, height)
		.build();
	assert(swapchainResult.has_value() && "Swapchain don't created");

	this->swapchain = swapchainResult.value();

	this->swapChainImages = this->swapchain.get_images().value();
	this->swapChainImageViews = this->swapchain.get_image_views().value();
}

void Swapchain::Recreate(vkb::Device device, VkFormat format, GECS::u32 width, GECS::u32 height) {
	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.format = format;
	surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

	vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ device }
		.set_old_swapchain(this->swapchain)
		.set_desired_format(surfaceFormat)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.build();
	assert(swapchainResult.has_value() && "Swapchain don't created");

	vkb::destroy_swapchain(this->swapchain);
	for (VkImageView imageView : this->swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	this->swapchain = swapchainResult.value();

	this->swapChainImages = this->swapchain.get_images().value();
	this->swapChainImageViews = this->swapchain.get_image_views().value();
}

void Swapchain::Destroy(VkDevice logDevice) {
	for (GECS::u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(logDevice, frameSyncs.fenceInFlight[i], nullptr);
		vkDestroySemaphore(logDevice, frameSyncs.semaphoreImageAvailable[i], nullptr);
		vkDestroySemaphore(logDevice, frameSyncs.semaphoreRenderFinished[i], nullptr);
	}

	for (VkImageView imageView : this->swapChainImageViews) {
		vkDestroyImageView(logDevice, imageView, nullptr);
	}
	swapChainImageViews.clear();

	vkb::destroy_swapchain(this->swapchain);
}

VkImage Swapchain::AcquireImage(VkDevice logDevice, GECS::u32 currentImage, GECS::u32& swapchainIndexImage) {
	const VkResult result = vkAcquireNextImageKHR(logDevice,
		this->swapchain,
		UINT64_MAX,
		this->frameSyncs.semaphoreImageAvailable[currentImage],
		VK_NULL_HANDLE,
		&swapchainIndexImage);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
		return VK_NULL_HANDLE;

	if (result != VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		assert(false && "Swapchain: image doesn't acquired");

	return this->swapChainImages[swapchainIndexImage];
}

VkResult Swapchain::Submit2AndPresent(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, GECS::u32 currentImage, GECS::u32* swapchainIndexImage) {
	VkCommandBufferSubmitInfo cmdSubmitInfo;
	cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	cmdSubmitInfo.commandBuffer = commandBuffer;

	VkSemaphoreSubmitInfo waitImageInfo = StructCreators::SemaphoreSubmitInfo(
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, this->frameSyncs.semaphoreImageAvailable[currentImage]
	);
	VkSemaphoreSubmitInfo waitRenderInfo = StructCreators::SemaphoreSubmitInfo(
		VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, this->frameSyncs.semaphoreRenderFinished[currentImage]
	);

	VkSubmitInfo2 submitInfo = StructCreators::SubmitInfo(&cmdSubmitInfo, &waitImageInfo, &waitRenderInfo);
	vkQueueSubmit2(graphicsQueue, 1, &submitInfo, this->frameSyncs.fenceInFlight[currentImage]);

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &(this->frameSyncs.semaphoreRenderFinished[currentImage]);
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &(this->swapchain.swapchain);
	presentInfo.pImageIndices = swapchainIndexImage;

	return vkQueuePresentKHR(graphicsQueue, &presentInfo);
}

VkResult Swapchain::WaitFences(VkDevice logDevice, GECS::u32 currentFrame) {
	return vkWaitForFences(logDevice, 1, &(this->frameSyncs.fenceInFlight[currentFrame]), VK_TRUE, UINT64_MAX);
}

VkResult Swapchain::ResetFences(VkDevice logDevice, GECS::u32 currentFrame) {
	return vkResetFences(logDevice, 1, &(this->frameSyncs.fenceInFlight[currentFrame]));
}