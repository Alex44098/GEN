#pragma once

#include <vec2.hpp>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Renderer/RenderTypes.h"

struct Image {
	BindlessId id{ INVALID_BINDLESS_ID };

	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkFormat format;
	VkImageUsageFlags usage;
	VkExtent3D extent;
	GECS::u32 mipLevels{ 1 };
	GECS::u32 numLayers{ 1 };

	glm::ivec2 getSize2D() const { return glm::ivec2{ this->extent.width, this->extent.height }; }
	VkExtent2D getExtent2D() const { return VkExtent2D{ this->extent.width, this->extent.height }; }
};