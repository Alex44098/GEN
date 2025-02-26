#include "Renderer/Vulkan/StructCreators/VkCommandStructs.h"

namespace StructCreators {
    VkCommandPoolCreateInfo CommandPoolInfo(
        VkCommandPoolCreateFlags flags,
        GECS::i32 queueFamilyIndex) {

        VkCommandPoolCreateInfo commandCreateInfo;
        commandCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandCreateInfo.flags = flags;
        commandCreateInfo.queueFamilyIndex = queueFamilyIndex;

        return commandCreateInfo;
    }

    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
        VkCommandPool commandPool,
        GECS::i32 commandBufferCount) {

        VkCommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = commandBufferCount;

        return commandBufferInfo;
    }
}