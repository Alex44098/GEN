#pragma once

#include <vulkan/vulkan.h>

namespace StructCreators {
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectFlags);
}