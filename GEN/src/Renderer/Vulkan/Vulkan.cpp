#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/Vulkan/StructCreators/VkBootstrapStructs.h"
#include "Renderer/Vulkan/StructCreators/VMAStructs.h"
#include "Renderer/Vulkan/StructCreators/VkCommandStructs.h"

#include <SDL.h>
#include <SDL_vulkan.h>

namespace gvk {
	Vulkan::Vulkan() {}

	void Vulkan::InitVulkan(SDL_Window* window) {
		volkInitialize();

		this->instance = vkb::InstanceBuilder{}
			.request_validation_layers()
			.use_default_debug_messenger()
			.require_api_version(1, 3, 0)
			.build()
			.value();

		volkLoadInstance(this->instance);

		SDL_bool surfaceResult = SDL_Vulkan_CreateSurface(window, this->instance, &this->surface);
		if (surfaceResult != SDL_TRUE)
			assert(false && "Vulkan surface not created");

		this->phDevice = vkb::PhysicalDeviceSelector{ this->instance }
			.set_minimum_version(1, 3)
			.set_required_features(StructCreators::PhysicalDeviceFeatures())
			.set_required_features_12(StructCreators::Vulkan12Features())
			.set_required_features_13(StructCreators::Vulkan13Features())
			.set_surface(this->surface)
			.select()
			.value();

		this->logDevice = vkb::DeviceBuilder{ phDevice }.build().value();

		this->graphicsQueueFamily = this->logDevice.get_queue_index(vkb::QueueType::graphics).value();
		this->graphicsQueue = this->logDevice.get_queue(vkb::QueueType::graphics).value();

		// Init VMA
		VmaVulkanFunctions vmaVkFunctions = StructCreators::VmaVkFunctions();

		VmaAllocatorCreateInfo vmaAllocatorInfo =
			StructCreators::VmaAllocatorInfo(this->instance, this->phDevice, this->logDevice, &vmaVkFunctions);

		vmaCreateAllocator(&vmaAllocatorInfo, &this->vkAllocator);
	}

	void Vulkan::CreateSwapchain(GECS::u64 width, GECS::u64 height) {
		VkSurfaceFormatKHR surfaceFormat;
		surfaceFormat.format = this->swapchainFormat;
		surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

		vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ this->logDevice }
			.set_desired_format(surfaceFormat)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(width, height)
			.build();
		assert(swapchainResult.has_value() && "Swapchain don't created");

		this->swapchain = swapchainResult.value();

		this->swapChainImages = this->swapchain.get_images().value();
		this->swapChainImageViews = this->swapchain.get_image_views().value();
	}

	void Vulkan::RecreateSwapchain(GECS::u64 width, GECS::u64 height) {
		VkSurfaceFormatKHR surfaceFormat;
		surfaceFormat.format = this->swapchainFormat;
		surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

		vkb::Result<vkb::Swapchain> swapchainResult = vkb::SwapchainBuilder{ this->logDevice }
			.set_old_swapchain(this->swapchain)
			.set_desired_format(surfaceFormat)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(width, height)
			.build();
		assert(swapchainResult.has_value() && "Swapchain don't created");

		vkb::destroy_swapchain(this->swapchain);
		for (VkImageView imageView : this->swapChainImageViews)
			vkDestroyImageView(this->logDevice, imageView, nullptr);

		this->swapchain = swapchainResult.value();

		this->swapChainImages = this->swapchain.get_images().value();
		this->swapChainImageViews = this->swapchain.get_image_views().value();
	}

	void Vulkan::InitSwapchainSyncs() {
		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (GECS::i32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkCreateFence(this->logDevice, &fenceCreateInfo, nullptr, &this->frameSyncs.fenceInFlight[i]);
			vkCreateSemaphore(this->logDevice, &semaphoreCreateInfo, nullptr, &this->frameSyncs.semaphoreImageAvailable[i]);
			vkCreateSemaphore(this->logDevice, &semaphoreCreateInfo, nullptr, &this->frameSyncs.semaphoreRenderFinished[i]);
		}
	}

	void Vulkan::CreateCommandBuffers() {
		const VkCommandPoolCreateInfo poolCreateInfo =
			StructCreators::CommandPoolInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, this->graphicsQueueFamily);

		for (GECS::i32 imageIndex = 0; imageIndex < MAX_FRAMES_IN_FLIGHT; imageIndex++) {
			VkCommandPool& commandPool = this->imageCommandPools[imageIndex];
			vkCreateCommandPool(this->logDevice, &poolCreateInfo, nullptr, &commandPool);
			
			VkCommandBufferAllocateInfo commandAllocInfo =
				StructCreators::CommandBufferAllocateInfo(commandPool, 1);
			VkCommandBuffer& commandBuffer = this->imageCommandBuffers[imageIndex];
			vkAllocateCommandBuffers(this->logDevice, &commandAllocInfo, &commandBuffer);
		}
	}

	void Vulkan::Init(SDL_Window* window) {
		this->InitVulkan(window);

		InitSwapchainSyncs();

		GECS::i32 width, height;
		SDL_GetWindowSize(window, &width, &height);
		CreateSwapchain(width, height);

		CreateCommandBuffers();
	}

	VkCommandBuffer Vulkan::StartFrameBuilding() {
		vkWaitForFences(this->logDevice, 1, &this->frameSyncs.fenceInFlight[this->currentImage], VK_TRUE, UINT64_MAX);

		VkCommandBufferBeginInfo cmdBeginInfo;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer& imageCommandBuffer = this->imageCommandBuffers[this->currentImage];
		vkBeginCommandBuffer(imageCommandBuffer, &cmdBeginInfo);

		return imageCommandBuffer;
	}

	void Vulkan::EndFrameBuilding() {
		// thinks about offscreen rendering
	}
}