#include "Renderer/Vulkan/Swapchain.h"
#include <iostream>

void Swapchain::Init(VkDevice logDevice) {
	const VkFenceCreateInfo fenceCreateInfo {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	const VkSemaphoreCreateInfo semaphoreCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	for (GECS::i32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VK_CHECK(vkCreateFence(logDevice, &fenceCreateInfo, nullptr, &(this->frameSyncs.fenceInFlight[i])));
		VK_CHECK(vkCreateSemaphore(logDevice, &semaphoreCreateInfo, nullptr, &(this->frameSyncs.semaphoreImageAvailable[i])));
		VK_CHECK(vkCreateSemaphore(logDevice, &semaphoreCreateInfo, nullptr, &(this->frameSyncs.semaphoreRenderFinished[i])));
	}
}

void Swapchain::Create(vkb::Device device, VkFormat format, GECS::u32 width, GECS::u32 height) {
	VkSurfaceFormatKHR surfaceFormat{
		.format = format,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	};

	vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ device }
		.set_desired_format(surfaceFormat)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // vsync
		.set_desired_extent(width, height)
		.build();

	if (!swapchainResult.has_value()) {
		std::cout << "Failed to create swapchain: " << std::endl;
		std::cout << swapchainResult.full_error().type.message() << std::endl;
		assert(false && "Swapchain don't created");
	}

	this->swapchain = swapchainResult.value();

	this->swapChainImages = this->swapchain.get_images().value();
	this->swapChainImageViews = this->swapchain.get_image_views().value();
}

void Swapchain::Recreate(vkb::Device device, VkFormat format, GECS::u32 width, GECS::u32 height) {
	assert(this->swapchain);
	
	VkSurfaceFormatKHR surfaceFormat{
		.format = format,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	};

	vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ device }
		.set_old_swapchain(this->swapchain)
		.set_desired_format(surfaceFormat)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.build();

	if (!swapchainResult.has_value()) {
		std::cout << "Failed to create swapchain: " << std::endl;
		std::cout << swapchainResult.full_error().type.message() << std::endl;
		assert(false && "Swapchain don't created");
	}

	vkb::destroy_swapchain(this->swapchain);
	for (VkImageView imageView : this->swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	this->swapchain = swapchainResult.value();

	this->swapChainImages = this->swapchain.get_images().value();
	this->swapChainImageViews = this->swapchain.get_image_views().value();

	this->needsRecreation = false;
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
	this->swapChainImageViews.clear();
	this->swapChainImages.clear();

	vkb::destroy_swapchain(this->swapchain);
}

VkImage Swapchain::AcquireImage(VkDevice logDevice, GECS::u32 currentImage, GECS::u32& swapchainIndexImage) {
	const VkResult res = vkAcquireNextImageKHR
	(
		logDevice,
		this->swapchain.swapchain,
		UINT64_MAX,
		this->frameSyncs.semaphoreImageAvailable[currentImage],
		VK_NULL_HANDLE,
		&swapchainIndexImage
	);

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
		this->needsRecreation = true;
		return VK_NULL_HANDLE;
		//return this->swapChainImages[swapchainIndexImage];
	}

	if (res != VK_SUCCESS)
		assert(false && "Swapchain: image doesn't acquired");

	return this->swapChainImages[swapchainIndexImage];
}

void Swapchain::Submit2AndPresent(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, GECS::u32 currentImage, GECS::u32 swapchainIndexImage) {
	
	const VkCommandBufferSubmitInfo cmdSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = commandBuffer,
	};
	const VkSemaphoreSubmitInfo waitImageInfo = StructCreators::SemaphoreSubmitInfo(
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
		this->frameSyncs.semaphoreImageAvailable[currentImage]
	);
	const VkSemaphoreSubmitInfo waitRenderInfo = StructCreators::SemaphoreSubmitInfo(
		VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
		this->frameSyncs.semaphoreRenderFinished[currentImage]
	);

	const VkSubmitInfo2 submitInfo = StructCreators::SubmitInfo(&cmdSubmitInfo, &waitImageInfo, &waitRenderInfo);
	//VkResult subRes = vkQueueSubmit2(graphicsQueue, 1, &submitInfo, this->frameSyncs.fenceInFlight[currentImage]);
	//VK_CHECK(subRes);
	VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, this->frameSyncs.fenceInFlight[currentImage]));

	const VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(this->frameSyncs.semaphoreRenderFinished[currentImage]),
		.swapchainCount = 1,
		.pSwapchains = &(this->swapchain.swapchain),
		.pImageIndices = &swapchainIndexImage
	};

	VkResult res = vkQueuePresentKHR(graphicsQueue, &presentInfo);
	if (res != VK_SUCCESS) {
		if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
			this->needsRecreation = true;
		}
		else {
			assert(false && "Swapchain: image didn't presented");
		}
	}
}

VkResult Swapchain::WaitFences(VkDevice logDevice, GECS::u32 currentFrame) {
	return vkWaitForFences(logDevice, 1, &(this->frameSyncs.fenceInFlight[currentFrame]), VK_TRUE, UINT64_MAX);
}

VkResult Swapchain::ResetFences(VkDevice logDevice, GECS::u32 currentFrame) {
	return vkResetFences(logDevice, 1, &(this->frameSyncs.fenceInFlight[currentFrame]));
}