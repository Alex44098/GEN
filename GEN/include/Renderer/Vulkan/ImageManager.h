#pragma once

#include <filesystem>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Image.h"
#include "Renderer/Vulkan/BindlessManager.h"

#include "Renderer/Vulkan/Util/MipMapGenerator.h"
#include "Renderer/Vulkan/Util/PipelineImageTransition.h"
#include "Renderer/Vulkan/Util/STBImageLoader.h"

namespace gvk {
    class Vulkan;
}

class ImageManager {
    struct LoadedImageInfo {
        std::filesystem::path path;
        VkFormat format;
        VkImageUsageFlags usage;
        bool mipMap;
    };

    gvk::Vulkan& vulkanInstance;

	std::vector<Image> images;
    std::unordered_map<ImageId, LoadedImageInfo> loadedImagesInfo;

    BindlessManager bindlessManager;

public:
    ImageManager(gvk::Vulkan& vulkan);
    void InitSamplers(float maxAnisotropy);

    void Clear();

    ImageId LoadImageFromFile(const std::filesystem::path& path, VkFormat format, VkImageUsageFlags usage, bool mipMap);
    ImageId CreateImage(const CreateImageInfo& createInfo, void* data, ImageId id);

    const Image& GetImage(ImageId id) const;

private:
    Image AllocateImage(const CreateImageInfo& createInfo) const;
    void LoadToGPU(const Image& image, void* data, GECS::u32 layer) const;

    ImageId PushToMemory(ImageId id, Image image);

    void DestroyImage(const Image& image);
};