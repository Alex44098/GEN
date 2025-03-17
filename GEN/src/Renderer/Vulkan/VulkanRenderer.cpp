#include "Renderer/Vulkan/VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(MeshManager& meshManager, MaterialManager& materialManager) :
	meshManager(meshManager), materialManager(materialManager)
{}

void VulkanRenderer::Init(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize) {
	this->InitSceneData(vulkan);
    this->CreateImages(vulkan, drawImageSize);

    this->samples = vulkan.GetMaxSampleCount();
}

void VulkanRenderer::Destroy(gvk::Vulkan& vulkan) {
    this->lightDataBuffer.Cleanup(vulkan);
    this->sceneDataBuffer.Cleanup(vulkan);
}

void VulkanRenderer::InitSceneData(gvk::Vulkan& vulkan) {
	this->sceneDataBuffer.Init(vulkan,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		sizeof(Gltf::GLTFSceneData));

	this->lightDataBuffer.Init(vulkan,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		sizeof(Gltf::GLTFLightData));
}

void VulkanRenderer::CreateImages(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize) {
    const VkExtent3D drawImageExtent {
        .width = (std::uint32_t)drawImageSize.x,
        .height = (std::uint32_t)drawImageSize.y,
        .depth = 1,
    };

    VkImageUsageFlags usages{};
    usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    usages |= VK_IMAGE_USAGE_SAMPLED_BIT;

    CreateImageInfo createDrawImageInfo {
        .format = this->drawImageFormat,
        .usage = usages,
        .extent = drawImageExtent,
        .samples = this->samples,
    };
    
    this->drawImageId = vulkan.GetImageManager().CreateImage(createDrawImageInfo, nullptr, this->drawImageId);

    if (!this->resolveImagesInitialized) {
        createDrawImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        this->postFXDrawImageId = vulkan.GetImageManager().CreateImage(createDrawImageInfo, nullptr, INVALID_IMAGE_ID);
    }

    if (!this->resolveImagesInitialized) { // setup resolve image
        VkImageUsageFlags usages{};
        usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        usages |= VK_IMAGE_USAGE_SAMPLED_BIT;

        const CreateImageInfo createImageInfo {
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .usage = usages,
            .extent = drawImageExtent,
        };
        this->resolveDrawImageId = vulkan.GetImageManager().CreateImage(createImageInfo, nullptr, INVALID_IMAGE_ID);
    }

    CreateImageInfo createInfo{
        .format = this->depthImageFormat,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .extent = drawImageExtent,
        .samples = this->samples,
    };

    this->depthImageId = vulkan.GetImageManager().CreateImage(createInfo, nullptr, this->depthImageId);

    if (!this->resolveImagesInitialized) {
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        this->resolveDepthImageId = vulkan.GetImageManager().CreateImage(createInfo, nullptr, INVALID_IMAGE_ID);

        this->resolveImagesInitialized = true;
    }
}

void VulkanRenderer::SortRenderingUnits() {
    this->renderingUnitsOrder.clear();
    this->renderingUnitsOrder.resize(this->renderingUnits.size());

    std::sort(this->renderingUnitsOrder.begin(), this->renderingUnitsOrder.end(),
        [this](const auto& i1, const auto& i2) {
            const GeometryRenderingUnit& gu1 = this->renderingUnits[i1];
            const GeometryRenderingUnit& gu2 = this->renderingUnits[i2];
            return gu1.meshId < gu2.meshId;
        });
}

void VulkanRenderer::StartFrameBuilding() {
    this->renderingUnits.clear();
    this->lightData.clear();
    this->sunlightIndex = -1;
}

void VulkanRenderer::EndFrameBuilding() {
    this->SortRenderingUnits();
}

void VulkanRenderer::AddRenderingUnit(MeshId meshId, MaterialId materialId, const glm::mat4& transform, bool castShadow) {
    assert(meshId != INVALID_MESH_ID && materialId != INVALID_MATERIAL_ID);

    GeometryRenderingUnit unit{
        .meshId = meshId,
        .materialId = materialId,
        .transformMatrix = transform,
        .castShadows = castShadow
    };

    this->renderingUnits.push_back(std::move(unit));
}

void VulkanRenderer::AddLight(const Gltf::GLTFLight& light, const Transform& transform) {
    // only sun can be directional
    if (light.type == Gltf::GLTFLightType::Directional)
        this->sunlightIndex = this->lightData.size();

    Gltf::GLTFLightData lightData;
    lightData.position - transform.GetPosition();
    lightData.type = light.GetTypeCode();
    lightData.direction = transform.GetLocalFront();
    if (light.range == 0) {
        if (light.type == Gltf::GLTFLightType::Point)
            lightData.range = 20.f;
        else if (light.type == Gltf::GLTFLightType::Spot)
            lightData.range = 60.f;
    }
    else
        lightData.range = light.range;

    LinearColorWithoutAlpha color{
        .r = light.color.r,
        .g = light.color.g,
        .b = light.color.b
    };
    lightData.color = color;

    lightData.intensity = light.intensity;
    lightData.scaleOffset.x = light.scaleOffset.x;
    lightData.scaleOffset.y = light.scaleOffset.y;

    this->lightData.push_back(std::move(lightData));
}

bool VulkanRenderer::MultisamplingEnabled() const {
    return this->samples != VK_SAMPLE_COUNT_1_BIT;
}

const Image& VulkanRenderer::GetDrawImage(gvk::Vulkan& vulkan) const {
    return vulkan.GetImageManager().GetImage(this->postFXDrawImageId);
}

const Image& VulkanRenderer::GetDepthImage(gvk::Vulkan& vulkan) const {
    return vulkan.GetImageManager().GetImage(this->MultisamplingEnabled()
        ? this->resolveDepthImageId : this->depthImageId);
}

VkFormat VulkanRenderer::GetDrawImageFormat() const {
    return this->drawImageFormat;
}

VkFormat VulkanRenderer::GetDepthImageFormat() const {
    return this->depthImageFormat;
}