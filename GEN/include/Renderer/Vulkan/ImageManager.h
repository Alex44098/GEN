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

	mutable std::vector<Image> images;
    mutable std::unordered_map<ImageId, LoadedImageInfo> loadedImagesInfo;

    mutable BindlessManager bindlessManager;
    
public:
    ImageManager(gvk::Vulkan& vulkan);
    void InitBindlessManager(VkDevice device);
    void InitSamplers(float maxAnisotropy);

    void Clear();

    ImageId LoadImageFromFile(const std::filesystem::path& path, VkFormat format, VkImageUsageFlags usage, bool mipMap) const;
    ImageId CreateImage(const CreateImageInfo& createInfo, void* data, ImageId id, const char* label) const;

    const Image& GetImage(ImageId id) const;
    inline VkDescriptorSetLayout GetDescSetLayout() const { return this->bindlessManager.getDescriptorSetLayout(); }
    inline const VkDescriptorSet& GetDescSet() const { return this->bindlessManager.getDescriptorSet(); }

    void CopyImage(
        VkCommandBuffer cmdBuffer,
        VkImage srcImage,
        VkImage destImage,
        VkExtent2D srcSize,
        int destX,
        int destY,
        int destW,
        int destH,
        VkFilter filter) const;

    Image AllocateImage(const CreateImageInfo& createInfo) const;

    void DestroyImage(const Image& image) const;

    void LoadToGPU(const Image& image, void* data, GECS::u32 layer) const;
private:

    ImageId PushToMemory(ImageId id, Image image) const;
};