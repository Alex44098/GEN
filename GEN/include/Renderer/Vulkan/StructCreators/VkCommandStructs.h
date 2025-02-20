#pragma once

#include <GECSHeaders.h>
#include <vulkan/vulkan.h>

namespace StructCreators {
    VkCommandPoolCreateInfo CommandPoolInfo(
        VkCommandPoolCreateFlags flags,
        GECS::i32 queueFamilyIndex);
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
        VkCommandPool commandPool,
        GECS::i32 commandBufferCount);
}