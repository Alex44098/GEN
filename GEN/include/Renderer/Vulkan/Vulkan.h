#pragma once

#include <vulkan/vulkan.hpp>
#include <Volk/volk.h>
#include <VkBootstrap/VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

#include <glm.hpp>

#include "Engine/EngineTypes.h"
#include "Renderer/Vulkan/Swapchain.h"

struct SDL_Window;

namespace gvk {

	struct FrameSync {
		VkSemaphore semaphoreImageAvailable[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore semaphoreRenderFinished[MAX_FRAMES_IN_FLIGHT];
		VkFence fenceInFlight[MAX_FRAMES_IN_FLIGHT];
	};

	class Vulkan {
		// simplifies memory allocation in Vulkan
		VmaAllocator vkAllocator;

		// vk-bootstrap wrappers
		vkb::Instance instance;
		vkb::PhysicalDevice phDevice;
		vkb::Device logDevice;

		// swapchain
		Swapchain swapchain;

		// command buffers for images
		VkCommandPool imageCommandPools[MAX_FRAMES_IN_FLIGHT];
		VkCommandBuffer imageCommandBuffers[MAX_FRAMES_IN_FLIGHT];

		// graphics objects
		VkSurfaceKHR surface;
		VkFormat swapchainFormat;
		GECS::i32 graphicsQueueFamily{ 0 };
		VkQueue graphicsQueue; // With present queue

		GECS::u32 currentImage{ 0 };

	public:
		Vulkan();
		Vulkan(const Vulkan&) = delete;
		Vulkan& operator=(const Vulkan&) = delete;

		void Init(SDL_Window* window);

		// for rendering
		VkCommandBuffer StartFrameBuilding();
		void EndFrameBuilding();

		// getters
		inline VkDevice GetDevice() const { return this->logDevice; }

		inline void IncreaseImageIndex() { currentImage = (currentImage + 1) % MAX_FRAMES_IN_FLIGHT; }

	private:
		void InitVulkan(SDL_Window* window);
		void CreateCommandBuffers();
	};
}