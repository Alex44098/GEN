#pragma once

#include <vec2.hpp>
#include <vulkan/vulkan.h>
#include "Renderer/Vulkan/VmaUsage.h"

#include "Renderer/RenderTypes.h"

struct Image {
	ImageId id{ INVALID_IMAGE_ID };

	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkFormat format;
	VkImageUsageFlags usage;
	VkExtent3D extent;
	GECS::u32 mipLevels{ 1 };
	GECS::u32 numLayers{ 1 };
	bool isCubemap{ false };

	glm::ivec2 getSize2D() const { return glm::ivec2{ this->extent.width, this->extent.height }; }
	VkExtent2D getExtent2D() const { return VkExtent2D{ this->extent.width, this->extent.height }; }
};