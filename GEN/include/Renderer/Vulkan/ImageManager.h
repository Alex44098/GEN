#pragma once

#include <filesystem>

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Image.h"

class ImageManager {
    struct LoadedImageInfo {
        std::filesystem::path path;
        VkFormat format;
        VkImageUsageFlags usage;
        bool mipMap;
    };

	std::vector<Image> images;
    std::unordered_map<ImageId, LoadedImageInfo> loadedImagesInfo;

    gvk::Vulkan& vulkanInstance;

public:
    ImageManager(gvk::Vulkan& vulkan);
    void Clear();

    ImageId LoadImageFromFile(const std::filesystem::path& path, VkFormat format, VkImageUsageFlags usage, bool mipMap);
    ImageId AddImage(Image image);

    const Image& GetImage(ImageId id) const;
};