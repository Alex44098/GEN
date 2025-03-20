#include "Renderer/Vulkan/StructCreators/VkCommandStructs.h"

namespace StructCreators {
    const VkCommandPoolCreateInfo CommandPoolInfo(
        VkCommandPoolCreateFlags flags,
        GECS::u32 queueFamilyIndex) {

        const VkCommandPoolCreateInfo commandCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = flags,
            .queueFamilyIndex = queueFamilyIndex
        };

        return commandCreateInfo;
    }

    const VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
        VkCommandPool commandPool,
        GECS::u32 commandBufferCount) {

        const VkCommandBufferAllocateInfo commandBufferInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = commandBufferCount
        };

        return commandBufferInfo;
    }
}