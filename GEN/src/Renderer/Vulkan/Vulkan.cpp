//#define VK_NO_PROTOTYPES
//#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>
#include "Renderer/Vulkan/VmaUsage.h"

#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/Vulkan/StructCreators/VkBootstrapStructs.h"
#include "Renderer/Vulkan/StructCreators/VMAStructs.h"
#include "Renderer/Vulkan/StructCreators/VkCommandStructs.h"
#include "Renderer/Vulkan/StructCreators/VkImageSubresourceRange.h"
#include "Renderer/Vulkan/Util/ImageSaver.h"

#include <SDL.h>
#include <SDL_vulkan.h>

namespace gvk {
	Vulkan::Vulkan() : imageManager(*this) {}

	void Vulkan::Init(SDL_Window* window) {
		this->InitVulkan(window);

		this->swapchain.Init(this->logDevice);

		GECS::i32 width, height;
		SDL_GetWindowSize(window, &width, &height);

		this->swapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
		//this->swapchainFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		this->swapchain.Create(this->logDevice, this->swapchainFormat, width, height);

		// for commands recording
		CreateCommandBuffers();

		CreateImageCommandBuffers();

		this->imageManager.InitBindlessManager(this->logDevice);
		this->imageManager.InitSamplers(this->maxAnisotropy);
	}

	void Vulkan::InitVulkan(SDL_Window* window) {
		VK_CHECK(volkInitialize());

		this->instance = vkb::InstanceBuilder{}
			.set_app_name("GEN")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.require_api_version(1, 3, 0)
			.build()
			.value();

			#if _DEBUG
				this->vkDebugMessenger = this->instance.debug_messenger;
			#endif

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
		// this->highestSupportedSamples = VK_SAMPLE_COUNT_1_BIT;

		this->logDevice = vkb::DeviceBuilder{ phDevice }.build().value();

		this->graphicsQueueFamily = this->logDevice.get_queue_index(vkb::QueueType::graphics).value();
		this->graphicsQueue = this->logDevice.get_queue(vkb::QueueType::graphics).value();

		// Init VMA
		const VmaVulkanFunctions vmaVkFunctions{
			// !!! funcs from volk.h !!!
			.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = vkGetDeviceProcAddr
		};

		const VmaAllocatorCreateInfo vmaAllocatorInfo =
			StructCreators::VmaAllocatorInfo(
				this->instance.instance,
				this->phDevice.physical_device,
				this->logDevice.device,
				vmaVkFunctions);

		VK_CHECK(vmaCreateAllocator(&vmaAllocatorInfo, &(this->vkAllocator)));
	}

	void Vulkan::Destroy() {
		this->imageManager.Clear();

		for (GECS::u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			vkDestroyCommandPool(this->logDevice.device, this->imageCommandPools[i], 0);

		swapchain.Destroy(this->logDevice.device);

		vkDestroyCommandPool(this->logDevice, this->commandPool, nullptr);
		vkDestroyFence(this->logDevice, this->commandFence, nullptr);

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
		VK_CHECK(vkCreateCommandPool(this->logDevice, &poolCreateInfo, nullptr, &(this->commandPool)));

		const VkCommandBufferAllocateInfo commandAllocInfo = StructCreators::CommandBufferAllocateInfo(commandPool, 1);
		VK_CHECK(vkAllocateCommandBuffers(this->logDevice, &commandAllocInfo, &(this->commandBuffer)));

		const VkFenceCreateInfo fenceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		VK_CHECK(vkCreateFence(this->logDevice.device, &fenceCreateInfo, nullptr, &(this->commandFence)));
	}

	void Vulkan::CreateImageCommandBuffers() {
		const VkCommandPoolCreateInfo poolCreateInfo =
			StructCreators::CommandPoolInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, this->graphicsQueueFamily);

		for (GECS::i32 imageIndex = 0; imageIndex < MAX_FRAMES_IN_FLIGHT; imageIndex++) {
			VkCommandPool& commandPool = this->imageCommandPools[imageIndex];
			VK_CHECK(vkCreateCommandPool(this->logDevice, &poolCreateInfo, nullptr, &commandPool));
			
			const VkCommandBufferAllocateInfo commandAllocInfo =
				StructCreators::CommandBufferAllocateInfo(commandPool, 1U);
			VkCommandBuffer& commandBuffer = this->imageCommandBuffers[imageIndex];
			VK_CHECK(vkAllocateCommandBuffers(this->logDevice, &commandAllocInfo, &commandBuffer));
		}
	}

	VkCommandBuffer Vulkan::BeginCommandBufferRecord() {
		VK_CHECK(vkResetFences(this->logDevice.device, 1, &(this->commandFence)));
		VK_CHECK(vkResetCommandBuffer(this->commandBuffer, 0));

		VkCommandBuffer cmd = this->commandBuffer;
		const VkCommandBufferBeginInfo cmdBeginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};

		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		return cmd;
	}

	void Vulkan::EndCommandBufferRecord(VkCommandBuffer cmd) {
		VK_CHECK(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdSubmit{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = cmd
		};

		const VkSubmitInfo2 submit = StructCreators::SubmitInfo(&cmdSubmit, nullptr, nullptr);

		VK_CHECK(vkQueueSubmit2(this->graphicsQueue, 1, &submit, this->commandFence));
		VK_CHECK(vkWaitForFences(this->logDevice, 1, &(this->commandFence), VK_TRUE, UINT64_MAX));
	}

	VkCommandBuffer& Vulkan::StartFrameBuilding() {
		VK_CHECK(this->swapchain.WaitFences(this->logDevice.device, this->currentFrame));

		const VkCommandBufferBeginInfo cmdBeginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};

		VkCommandBuffer& imageCommandBuffer = this->imageCommandBuffers[this->currentFrame];
		VK_CHECK(vkBeginCommandBuffer(imageCommandBuffer, &cmdBeginInfo));

		return imageCommandBuffer;
	}

	void Vulkan::EndFrameBuilding(
		VkCommandBuffer cmdBuffer,
		const ImageId drawImageId,
		const LinearColor clearColor,
		bool copyImageIntoSwapchain,
		glm::ivec4 drawImageBlitRect,
		bool drawImageLinearBlit) {		

		// thinks about offscreen rendering
		GECS::u32 swapchainImageIndex{ 0 };
		const VkImage swapchainImage = this->swapchain.AcquireImage(this->logDevice.device, this->currentFrame, swapchainImageIndex);
		if (swapchainImage == VK_NULL_HANDLE) {
			VK_CHECK(vkEndCommandBuffer(cmdBuffer));
			return;
		}

		VK_CHECK(this->swapchain.ResetFences(this->logDevice.device, this->currentFrame));
		
		VkImageLayout swapchainLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkImageSubresourceRange clearRange = StructCreators::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		Util::PipelineImageTransition
		(
			cmdBuffer,
			swapchainImage,
			swapchainLayout,
			VK_IMAGE_LAYOUT_GENERAL
		);
		swapchainLayout = VK_IMAGE_LAYOUT_GENERAL;

		const VkClearColorValue clearValue{
			clearColor.r, clearColor.g, clearColor.b
		};
		vkCmdClearColorImage(cmdBuffer, swapchainImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

		if (copyImageIntoSwapchain) {
			const Image& drawImage = this->imageManager.GetImage(drawImageId);

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

		VK_CHECK(vkEndCommandBuffer(cmdBuffer));

		// Util::SaveImage(*this, drawImage);

		this->swapchain.Submit2AndPresent(
			cmdBuffer,
			this->graphicsQueue,
			this->currentFrame,
			swapchainImageIndex
		);

		this->IncreaseImageIndex();
	}

	void Vulkan::RecreateSwapchain(GECS::u32 w, GECS::u32 h) {
		this->WaitIdle();
		this->swapchain.Recreate(this->logDevice, this->swapchainFormat, w, h);
	}

	Buffer Vulkan::CreateBuffer(std::size_t size, VkBufferUsageFlags vkUsage, VmaMemoryUsage vmaUsage) const {
		const VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = vkUsage
		};

		const VmaAllocationCreateInfo allocInfo{
			.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			.usage = vmaUsage
		};

		Buffer buffer{};
		VK_CHECK(vmaCreateBuffer(this->vkAllocator, &bufferInfo, &allocInfo,
			&buffer.vkBuffer, &buffer.allocation, &buffer.allocInfo));

		// create address for shaders
		if ((vkUsage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0) {
			const VkBufferDeviceAddressInfo deviceAddressInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = buffer.vkBuffer
			};

			buffer.address = vkGetBufferDeviceAddress(this->logDevice.device, &deviceAddressInfo);
		}

		return buffer;
	}

	void Vulkan::DestroyBuffer(const Buffer& buffer) {
		vmaDestroyBuffer(this->vkAllocator, buffer.vkBuffer, buffer.allocation);
	}

	void Vulkan::WaitIdle() const {
		VK_CHECK(vkDeviceWaitIdle(this->logDevice.device));
	}
}