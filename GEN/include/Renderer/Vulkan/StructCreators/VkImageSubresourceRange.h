#pragma once

#include <Volk/volk.h>

namespace StructCreators {
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectFlags);
}