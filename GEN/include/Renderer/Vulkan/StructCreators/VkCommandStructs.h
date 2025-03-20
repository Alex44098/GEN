#pragma once

#include <GECSHeaders.h>
#include <Volk/volk.h>

namespace StructCreators {
    const VkCommandPoolCreateInfo CommandPoolInfo(
        VkCommandPoolCreateFlags flags,
        GECS::u32 queueFamilyIndex);
    const VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
        VkCommandPool commandPool,
        GECS::u32 commandBufferCount);
}