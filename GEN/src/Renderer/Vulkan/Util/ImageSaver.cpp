#include "Renderer/Vulkan/Util/ImageSaver.h"
#include "Renderer/Vulkan/Util/PipelineImageTransition.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <chrono>
#include <format>

namespace Util {
	void SaveImage(gvk::Vulkan& vulkan, const Image& drawImage) {
		const std::filesystem::path dirPath = "scr";

		if (!std::filesystem::exists(dirPath))
			return;

		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> time =
			std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());

		const std::string filename = std::format("{:%Y-%m-%d_%H-%M-%S}.png", time);
		const std::filesystem::path imagePath = dirPath / filename;

        const CreateImageInfo createImageInfo = {
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .extent =
            {
                .width = (std::uint32_t)drawImage.extent.width,
                .height = (std::uint32_t)drawImage.extent.height,
                .depth = 1,
            },
            .tiling = VK_IMAGE_TILING_LINEAR,
        };
        const auto allocInfo = VmaAllocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
        };
        const Image dstImage = vulkan.GetImageManager().AllocateImage(createImageInfo);

        VkCommandBuffer cmdBuffer = vulkan.BeginCommandBufferRecord();

        PipelineImageTransition
        (
            cmdBuffer,
            drawImage.image,
            //VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
        );
        PipelineImageTransition
        (
            cmdBuffer,
            dstImage.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );
        vulkan.GetImageManager().CopyImage
        (
            cmdBuffer,
            drawImage.image,
            dstImage.image,
            drawImage.getExtent2D(),
            0,
            0,
            dstImage.getExtent2D().width,
            dstImage.getExtent2D().height,
            VK_FILTER_NEAREST
        );

        vulkan.EndCommandBufferRecord(cmdBuffer);

        VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(
            vulkan.GetDevice(), dstImage.image, &subResource, &subResourceLayout);

        VmaAllocationInfo info;
        vmaGetAllocationInfo(vulkan.GetAllocator(), drawImage.allocation, &info);
        const char* data = (const char*)info.pMappedData;
        data += subResourceLayout.offset;

        stbi_write_force_png_filter = 0;
        int numChannels = 4;
        const auto res = stbi_write_png(
            imagePath.string().c_str(),
            dstImage.extent.width,
            dstImage.extent.height,
            numChannels,
            data,
            subResourceLayout.rowPitch);

        if (res == 0) {
            assert(false && "ImageSaver: image didn't saved");
        }

        vulkan.GetImageManager().DestroyImage(dstImage);
	}
}