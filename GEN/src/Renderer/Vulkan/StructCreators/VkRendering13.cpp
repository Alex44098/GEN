#include "Renderer/Vulkan/StructCreators/VkRendering13.h"

namespace StructCreators {
    RenderInfo CreateRenderingInfo(const RenderInfoParams& params) {
        RenderInfo renderInfo;
        if (params.colorImageView) {
            renderInfo.colorAttach = VkRenderingAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = params.colorImageView,
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp = params.colorImageClearValue
                ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            };
            if (params.colorImageClearValue) {
                const glm::vec4 col = params.colorImageClearValue.value();
                renderInfo.colorAttach.clearValue.color = { col[0], col[1], col[2], col[3] };
            }
        }

        if (params.depthImageView) {
            renderInfo.depthAttach = VkRenderingAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = params.depthImageView,
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                .loadOp = params.depthImageClearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR :
                                                        VK_ATTACHMENT_LOAD_OP_LOAD,
                //.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE
                //.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE
            };
            if (params.depthImageClearValue) {
                //renderInfo.depthAttach.clearValue.depthStencil.depth = params.depthImageClearValue.value();
                renderInfo.depthAttach.clearValue.depthStencil = {params.depthImageClearValue.value(), 0};
            }
        }

        if (params.resolveImageView) {
            renderInfo.colorAttach.resolveImageView = params.resolveImageView;
            renderInfo.colorAttach.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            renderInfo.colorAttach.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        }

        renderInfo.renderingInfo = VkRenderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea =
                VkRect2D{
                    .offset = {},
                    .extent = params.extent,
                },
            .layerCount = 1,
            .colorAttachmentCount = params.colorImageView ? 1U : 0U,
            .pColorAttachments = params.colorImageView ? &renderInfo.colorAttach : nullptr,
            .pDepthAttachment = params.depthImageView ? &renderInfo.depthAttach : nullptr
        };

        return renderInfo;
    }
}