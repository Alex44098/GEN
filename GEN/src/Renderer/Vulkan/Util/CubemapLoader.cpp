#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/Vulkan/Util/STBImageLoader.h"

namespace Util {


	ImageId LoadCubemap(gvk::Vulkan& vulkan, const std::filesystem::path& cubemapDir) {
		// const std::string sides[6]{"right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png"};
		const std::string sides[6]{ "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };

		VkImageUsageFlags usages{};
		usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		usages |= VK_IMAGE_USAGE_STORAGE_BIT;
		usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		ImageId cubemapImageId;

		for (GECS::u32 i = 0; i < 6; i++) {
			STBImage image = LoadSTBImage(cubemapDir / sides[i]);
			assert(image.channels == 4);
			assert(image.data != nullptr);

			if (i == 0) {
				const CreateImageInfo imageInfo{
					.format = VK_FORMAT_R8G8B8A8_SRGB,
					.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
					.extent =
						{
							.width = (std::uint32_t)image.width,
							.height = (std::uint32_t)image.height,
							.depth = 1,
						},
					.numLayers = 6,
					.isCubemap = true,
				};
				cubemapImageId = vulkan.GetImageManager().CreateImage(imageInfo, image.data, INVALID_IMAGE_ID, "Cubemap");
			}
			else {
				assert(vulkan.GetImageManager().GetImage(cubemapImageId).extent.width == (std::uint32_t)image.width &&
					vulkan.GetImageManager().GetImage(cubemapImageId).extent.height == (std::uint32_t)image.height &&
					"Cubemap loader: all images must have the same size");

				vulkan.GetImageManager().LoadToGPU(vulkan.GetImageManager().GetImage(cubemapImageId), image.data, i);
			}

			image.Destroy();
		}

		return cubemapImageId;
	}
}