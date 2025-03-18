#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>

#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/Vulkan/StructCreators/VkBootstrapStructs.h"
#include "Renderer/Vulkan/StructCreators/VMAStructs.h"
#include "Renderer/Vulkan/StructCreators/VkCommandStructs.h"
#include "Renderer/Vulkan/StructCreators/VkImageSubresourceRange.h"

#include <SDL.h>
#include <SDL_vulkan.h>

namespace gvk {
	Vulkan::Vulkan() : imageManager(*this) {}

	void Vulkan::Init(SDL_Window* window) {
		this->InitVulkan(window);

		this->imageManager.InitSamplers(this->maxAnisotropy);

		this->swapchain.Init(this->logDevice.device);

		GECS::i32 width, height;
		SDL_GetWindowSize(window, &width, &height);

		this->swapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
		this->swapchain.Create(this->logDevice, this->swapchainFormat, width, height);

		CreateCommandBuffers();
		CreateImageCommandBuffers();
	}

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

		this->SetSamplerParameters();

		this->logDevice = vkb::DeviceBuilder{ phDevice }.build().value();

		this->graphicsQueueFamily = this->logDevice.get_queue_index(vkb::QueueType::graphics).value();
		this->graphicsQueue = this->logDevice.get_queue(vkb::QueueType::graphics).value();

		// Init VMA
		VmaVulkanFunctions vmaVkFunctions = StructCreators::VmaVkFunctions();

		VmaAllocatorCreateInfo vmaAllocatorInfo =
			StructCreators::VmaAllocatorInfo(this->instance, this->phDevice, this->logDevice, &vmaVkFunctions);

		vmaCreateAllocator(&vmaAllocatorInfo, &(this->vkAllocator));
	}

	void Vulkan::Destroy() {
		for (GECS::u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			vkDestroyCommandPool(this->logDevice.device, this->imageCommandPools[i], 0);

		swapchain.Destroy(this->logDevice.device);

		vkb::destroy_surface(this->instance, this->surface);
		vmaDestroyAllocator(this->vkAllocator);
		vkb::destroy_device(this->logDevice);
		vkb::destroy_instance(this->instance);
	}

	void Vulkan::SetSamplerParameters() {
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(this->phDevice, &props);
		maxAnisotropy = props.limits.maxSamplerAnisotropy;

		const VkSampleCountFlagBits sampleCounts[7]
		{
			VK_SAMPLE_COUNT_1_BIT,
			VK_SAMPLE_COUNT_2_BIT,
			VK_SAMPLE_COUNT_4_BIT,
			VK_SAMPLE_COUNT_8_BIT,
			VK_SAMPLE_COUNT_16_BIT,
			VK_SAMPLE_COUNT_32_BIT,
			VK_SAMPLE_COUNT_64_BIT
		};

		const VkSampleCountFlags supportedByDepthAndColor =
			props.limits.framebufferColorSampleCounts & props.limits.framebufferDepthSampleCounts;

		for (const VkSampleCountFlagBits& count : sampleCounts) {
			if (supportedByDepthAndColor & count) {
				this->supportedSampleCounts = (VkSampleCountFlagBits)(supportedSampleCounts | count);
				this->highestSupportedSamples = count;
			}
		}
	}

	void Vulkan::CreateCommandBuffers() {
		const VkCommandPoolCreateInfo poolCreateInfo =
			StructCreators::CommandPoolInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, this->graphicsQueueFamily);
		vkCreateCommandPool(this->logDevice, &poolCreateInfo, nullptr, &(this->commandPool));

		const VkCommandBufferAllocateInfo commandAllocInfo = StructCreators::CommandBufferAllocateInfo(commandPool, 1);
		vkAllocateCommandBuffers(this->logDevice, &commandAllocInfo, &(this->commandBuffer));

		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		vkCreateFence(this->logDevice.device, &fenceCreateInfo, nullptr, &(this->commandFence));
	}

	void Vulkan::CreateImageCommandBuffers() {
		const VkCommandPoolCreateInfo poolCreateInfo =
			StructCreators::CommandPoolInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, this->graphicsQueueFamily);

		for (GECS::i32 imageIndex = 0; imageIndex < MAX_FRAMES_IN_FLIGHT; imageIndex++) {
			VkCommandPool& commandPool = this->imageCommandPools[imageIndex];
			vkCreateCommandPool(this->logDevice, &poolCreateInfo, nullptr, &commandPool);
			
			const VkCommandBufferAllocateInfo commandAllocInfo =
				StructCreators::CommandBufferAllocateInfo(commandPool, 1);
			VkCommandBuffer& commandBuffer = this->imageCommandBuffers[imageIndex];
			vkAllocateCommandBuffers(this->logDevice, &commandAllocInfo, &commandBuffer);
		}
	}

	VkCommandBuffer Vulkan::BeginCommandBufferRecord() {
		vkResetFences(this->logDevice.device, 1, &(this->commandFence));
		vkResetCommandBuffer(this->commandBuffer, 0);

		VkCommandBuffer cmd = this->commandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmd, &cmdBeginInfo);

		return cmd;
	}

	void Vulkan::EndCommandBufferRecord(VkCommandBuffer cmd) {
		vkEndCommandBuffer(cmd);

		VkCommandBufferSubmitInfo cmdSubmit;
		cmdSubmit.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		cmdSubmit.commandBuffer = cmd;

		VkSubmitInfo2 submit = StructCreators::SubmitInfo(&cmdSubmit, nullptr, nullptr);

		vkQueueSubmit2(this->graphicsQueue, 1, &submit, this->commandFence);
		vkWaitForFences(this->logDevice, 1, &(this->commandFence), VK_TRUE, UINT64_MAX);
	}

	VkCommandBuffer& Vulkan::StartFrameBuilding() {
		this->swapchain.WaitFences(this->logDevice.device, currentFrame);

		VkCommandBufferBeginInfo cmdBeginInfo;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer& imageCommandBuffer = this->imageCommandBuffers[this->currentFrame];
		vkBeginCommandBuffer(imageCommandBuffer, &cmdBeginInfo);

		return imageCommandBuffer;
	}

	void Vulkan::EndFrameBuilding(
		VkCommandBuffer cmdBuffer,
		const Image& drawImage,
		const LinearColor clearColor,
		bool copyImageIntoSwapchain,
		glm::ivec4 drawImageBlitRect,
		bool drawImageLinearBlit) {

		// thinks about offscreen rendering
		GECS::u32 swapchainImageIndex{ 0 };
		const VkImage swapchainImage = this->swapchain.AcquireImage(this->logDevice, this->currentFrame, swapchainImageIndex);
		if (swapchainImage == VK_NULL_HANDLE)
			return;

		this->swapchain.ResetFences(this->logDevice, this->currentFrame);
		
		VkImageLayout swapchainLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageSubresourceRange clearRange = StructCreators::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		Util::PipelineImageTransition(
			cmdBuffer, swapchainImage, swapchainLayout, VK_IMAGE_LAYOUT_GENERAL);
		swapchainLayout = VK_IMAGE_LAYOUT_GENERAL;
		const VkClearColorValue clearValue{
			clearColor.r, clearColor.g, clearColor.b
		};
		vkCmdClearColorImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

		if (copyImageIntoSwapchain) {
			Util::PipelineImageTransition(
				cmdBuffer,
				drawImage.image,
				VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			Util::PipelineImageTransition(
				cmdBuffer,
				swapchainImage,
				swapchainLayout,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			swapchainLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			const VkFilter filter = drawImageLinearBlit ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

			if (drawImageBlitRect != glm::ivec4{})
				this->imageManager.CopyImage(
					cmdBuffer,
					drawImage.image,
					swapchainImage,
					drawImage.getExtent2D(),
					drawImageBlitRect.x,
					drawImageBlitRect.y,
					drawImageBlitRect.z,
					drawImageBlitRect.w,
					filter);
			else {
				const VkExtent2D imageExtent = drawImage.getExtent2D();
				this->imageManager.CopyImage(
					cmdBuffer,
					drawImage.image,
					swapchainImage,
					imageExtent,
					0,
					0,
					imageExtent.width,
					imageExtent.height,
					filter);
			}
				
		}

		Util::PipelineImageTransition(
			cmdBuffer,
			swapchainImage,
			swapchainLayout,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		swapchainLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		vkEndCommandBuffer(cmdBuffer);
		this->swapchain.Submit2AndPresent(
			cmdBuffer,
			this->graphicsQueue,
			this->currentFrame,
			&swapchainImageIndex
		);

		this->IncreaseImageIndex();
	}

	Buffer Vulkan::CreateBuffer(std::size_t size, VkBufferUsageFlags vkUsage, VmaMemoryUsage vmaUsage) {
		VkBufferCreateInfo bufferInfo;
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = vkUsage;

		VmaAllocationCreateInfo allocInfo;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		allocInfo.usage = vmaUsage;

		Buffer buffer;
		vmaCreateBuffer(this->vkAllocator, &bufferInfo, &allocInfo,
			&buffer.vkBuffer, &buffer.allocation, &buffer.allocInfo);

		// create address for shaders
		if ((vkUsage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0) {
			VkBufferDeviceAddressInfo deviceAddressInfo;
			deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			deviceAddressInfo.buffer = buffer.vkBuffer;

			buffer.address = vkGetBufferDeviceAddress(this->logDevice.device, &deviceAddressInfo);
		}

		return buffer;
	}

	void Vulkan::DestroyBuffer(const Buffer& buffer) {
		vmaDestroyBuffer(this->vkAllocator, buffer.vkBuffer, buffer.allocation);
	}
}