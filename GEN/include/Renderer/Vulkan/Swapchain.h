#pragma once

#include <vulkan/vulkan.hpp>
#include <VkBootstrap/VkBootstrap.h>

#include "Renderer/Vulkan/StructCreators/Vk13SubmitStructs.h"
#include "Engine/EngineTypes.h"

class Swapchain {
	struct FrameSync {
		VkSemaphore semaphoreImageAvailable[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore semaphoreRenderFinished[MAX_FRAMES_IN_FLIGHT];
		VkFence fenceInFlight[MAX_FRAMES_IN_FLIGHT];
	};

	vkb::Swapchain swapchain;
	FrameSync frameSyncs;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

public:
	void Init(VkDevice logDevice);
	void Create(vkb::Device device, VkFormat format, GECS::u64 width, GECS::u64 height);
	void Recreate(vkb::Device device, VkFormat format, GECS::u64 width, GECS::u64 height);
	void Destroy(VkDevice logDevice);
	VkImage AcquireImage(VkDevice logDevice, GECS::u32 currentImage, GECS::u32* indexImage);
	VkResult Submit2AndPresent(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, GECS::u32 currentImage, GECS::u32* swapchainIndexImage);
	
	VkResult WaitFences(VkDevice logDevice, GECS::u32 currentFrame);
	VkResult ResetFences(VkDevice logDevice, GECS::u32 currentFrame);
};