#include "Renderer/Vulkan/ImageManager.h"

ImageManager::ImageManager(gvk::Vulkan& vulkan, float maxAnisotropy)
	: vulkanInstance(vulkan), bindlessManager(vulkan.GetDevice(), maxAnisotropy)
{}

void ImageManager::Clear() {
	for (const Image& image : this->images)
		this->DestroyImage(image);
	images.clear();
	loadedImagesInfo.clear();
}

void ImageManager::DestroyImage(const Image& image) {
	vkDestroyImageView(this->vulkanInstance.GetDevice(), image.imageView, nullptr);
	vmaDestroyImage(this->vulkanInstance.GetAllocator(), image.image, image.allocation);
}

const Image& ImageManager::GetImage(ImageId id) const {
	return this->images.at(id);
}

ImageId ImageManager::CreateImage(const CreateImageInfo& createInfo, void* data, ImageId id) {
	Image image = this->AllocateImage(createInfo);
	if (data)
		this->LoadToGPU(image, data, 0U);

	return this->PushToMemory(id, std::move(image));
}

ImageId ImageManager::PushToMemory(ImageId id, Image image) {
	if (id == INVALID_IMAGE_ID) {
		id = this->images.size();
		image.id = id;
		this->images.push_back(std::move(image));
	}
	else {
		image.id = id;
		this->images[id] = std::move(image);
	}
	this->bindlessManager.AddImage(id, image.imageView);
	return id;
}

Image ImageManager::AllocateImage(const CreateImageInfo& createInfo) const {
	GECS::u32 mipLevels = 1;
	if (createInfo.mipMap) {
		const GECS::u32 maxExtent = std::max(createInfo.extent.width, createInfo.extent.height);
		mipLevels = (GECS::u32)std::floor(std::log2(maxExtent)) + 1;
	}

	const VkImageCreateInfo imageInfo {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = createInfo.flags,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = createInfo.format,
		.extent = createInfo.extent,
		.mipLevels = mipLevels,
		.arrayLayers = createInfo.numLayers,
		.samples = createInfo.samples,
		.tiling = createInfo.tiling,
		.usage = createInfo.usage,
	};

	const VmaAllocationCreateInfo allocInfo {
		.usage = VMA_MEMORY_USAGE_AUTO,
		.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};

	Image image{};
	image.format = createInfo.format;
	image.usage = createInfo.usage;
	image.extent = createInfo.extent;
	image.mipLevels = mipLevels;
	image.numLayers = createInfo.numLayers;

	vmaCreateImage(this->vulkanInstance.GetAllocator(), &imageInfo, &allocInfo,
		&image.image, &image.allocation, nullptr);

	bool canCreateImageView = ((createInfo.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) ||
		((createInfo.usage & VK_IMAGE_USAGE_STORAGE_BIT) != 0) ||
		((createInfo.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0) ||
		((createInfo.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0);

	if (canCreateImageView) {
		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (createInfo.format == VK_FORMAT_D32_SFLOAT) {
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		auto viewType = createInfo.numLayers == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		const auto viewCreateInfo = VkImageViewCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image.image,
			.viewType = viewType,
			.format = createInfo.format,
			.subresourceRange =
				VkImageSubresourceRange{
					.aspectMask = aspectFlag,
					.baseMipLevel = 0,
					.levelCount = mipLevels,
					.baseArrayLayer = 0,
					.layerCount = createInfo.numLayers,
				},
		};

		vkCreateImageView(this->vulkanInstance.GetDevice(), &viewCreateInfo, nullptr, &image.imageView);
	}

	return image;
}

void ImageManager::LoadToGPU(const Image& image, void* data, GECS::u32 layer) const {
	GECS::u32 numChannels;
	// for normal map
	if (image.format == VK_FORMAT_R8_UNORM)
		numChannels = 1;
	else
		numChannels = 4;

	const GECS::u32 dataSize =
		image.extent.width * image.extent.height * image.extent.depth * numChannels;

	const Buffer staggingBuffer = this->vulkanInstance.CreateBuffer(dataSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO);
	memcpy(staggingBuffer.allocInfo.pMappedData, data, dataSize);

	// start recording commands
	VkCommandBuffer cmdBuffer = this->vulkanInstance.BeginCommandBufferRecord();

	assert((image.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0 &&
		"The image cannot accept the data");
	Util::PipelineImageTransition(cmdBuffer, image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	const VkBufferImageCopy copyRegion{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = 
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = layer,
			.layerCount = 1
		},
		.imageExtent = image.extent
	};
	vkCmdCopyBufferToImage(
		cmdBuffer,
		staggingBuffer.vkBuffer,
		image.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copyRegion
	);
	if (image.mipLevels > 1) {
		Util::GenerateMipMaps(cmdBuffer, image.image, VkExtent2D{image.extent.width, image.extent.height}, image.mipLevels);
	}
	else {
		Util::PipelineImageTransition(cmdBuffer, image.image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	this->vulkanInstance.EndCommandBufferRecord(cmdBuffer);

	this->vulkanInstance.DestroyBuffer(staggingBuffer);
}