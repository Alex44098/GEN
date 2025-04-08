#pragma once

//#define VK_NO_PROTOTYPES
//#include <vulkan/vulkan.h>
//#define VOLK_IMPLEMENTATION
#include "Volk/volk.h"

#include <VkBootstrap/VkBootstrap.h>
#include "Renderer/Vulkan/VmaUsage.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Swapchain.h"
#include "Renderer/Vulkan/Buffer.h"
#include "Renderer/Vulkan/ImageManager.h"

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

		// command buffer
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkFence commandFence;

		// graphics objects
		VkSurfaceKHR surface;
		VkFormat swapchainFormat;
		GECS::u32 graphicsQueueFamily{ 0 };
		VkQueue graphicsQueue; // With present queue
		GECS::u32 currentFrame{ 0 };

		// sampler variables
		VkSampleCountFlagBits supportedSampleCounts{};
		VkSampleCountFlagBits highestSupportedSamples{ VK_SAMPLE_COUNT_1_BIT };
		float maxAnisotropy{ 1.f };

		// managers
		ImageManager imageManager;

		// debuger
		#if _DEBUG
			VkDebugUtilsMessengerEXT vkDebugMessenger;
		#endif

	public:
		Vulkan();
		Vulkan(const Vulkan& o) = delete;
		Vulkan& operator=(const Vulkan& o) = delete;

		void Init(SDL_Window* window);
		void Destroy();

		Buffer CreateBuffer(std::size_t size, VkBufferUsageFlags vkUsage, VmaMemoryUsage vmaUsage);
		void DestroyBuffer(const Buffer& buffer);

		// commands recording
		VkCommandBuffer BeginCommandBufferRecord();
		void EndCommandBufferRecord(VkCommandBuffer cmd);

		// for rendering
		VkCommandBuffer& StartFrameBuilding();
		void EndFrameBuilding(
			VkCommandBuffer cmdBuffer,
			const ImageId drawImageId,
			const LinearColor clearColor,
			bool copyImageIntoSwapchain = true,
			glm::ivec4 drawImageBlitRect = glm::ivec4{},
			bool drawImageLinearBlit = true);

		// for swapchain
		void RecreateSwapchain(GECS::u32 w, GECS::u32 h);
		inline const bool SwapchainNeedsRecreation() const { return this->swapchain.NeedsRecreation(); }
		inline const glm::ivec2 GetSwapchainSize() const {
			return glm::ivec2{ this->swapchain.GetExtent().width, this->swapchain.GetExtent().height };
		}

		// getters
		inline GECS::u32 GetCurrentFrame() const { return this->currentFrame; }
		inline VkDevice GetDevice() const { return this->logDevice; }
		inline VmaAllocator GetAllocator() const { return this->vkAllocator; }
		inline const ImageManager& GetImageManager() const { return this->imageManager; }
		inline VkSampleCountFlagBits GetMaxSampleCount() const { return this->highestSupportedSamples; }

		// other
		void WaitIdle() const;

	private:
		void InitVulkan(SDL_Window* window);
		void SetSamplerParameters();
		void CreateCommandBuffers();
		void CreateImageCommandBuffers();

		inline void IncreaseImageIndex() { currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; }
	};
}