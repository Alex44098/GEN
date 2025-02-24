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

		SDL_bool surfaceResult = SDL_Vulkan_CreateSurface(window, this->instance, &(this->surface));
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

		vmaCreateAllocator(&vmaAllocatorInfo, &(this->vkAllocator));
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

		this->swapchain.Init(this->logDevice.device);

		GECS::i32 width, height;
		SDL_GetWindowSize(window, &width, &height);

		this->swapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
		this->swapchain.Create(this->logDevice, this->swapchainFormat, width, height);

		CreateCommandBuffers();
	}

	VkCommandBuffer Vulkan::StartFrameBuilding() {
		this->swapchain.WaitFences(this->logDevice.device, currentImage);

		VkCommandBufferBeginInfo cmdBeginInfo;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer& imageCommandBuffer = this->imageCommandBuffers[this->currentImage];
		vkBeginCommandBuffer(imageCommandBuffer, &cmdBeginInfo);

		return imageCommandBuffer;
	}

	void Vulkan::EndFrameBuilding() {
		// thinks about offscreen rendering



		this->IncreaseImageIndex();
	}
}