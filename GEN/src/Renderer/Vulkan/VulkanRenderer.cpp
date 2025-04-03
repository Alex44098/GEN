#include "Renderer/Vulkan/VulkanRenderer.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

#include <numeric>

VulkanRenderer::VulkanRenderer(MeshManager& meshManager, MaterialManager& materialManager) :
	meshManager(meshManager), materialManager(materialManager)
{}

void VulkanRenderer::Init(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize) {
    this->samples = vulkan.GetMaxSampleCount();
    
    this->InitSceneData(vulkan);
    this->CreateImages(vulkan, drawImageSize);

    this->meshPipeline.Init(vulkan, drawImageFormat, depthImageFormat, samples);
    this->skyboxPipeline.Init(vulkan, drawImageFormat, depthImageFormat, samples);
    this->depthResolvePipeline.Init(vulkan, drawImageFormat, depthImageFormat, samples);
    this->postFXPipeline.Init(vulkan, drawImageFormat, depthImageFormat, samples);
}

void VulkanRenderer::Destroy(gvk::Vulkan& vulkan) {
    this->lightDataBuffer.Cleanup(vulkan);
    this->sceneDataBuffer.Cleanup(vulkan);

    const VkDevice device = vulkan.GetDevice();
    this->meshPipeline.Cleanup(device);
    this->skyboxPipeline.Cleanup(device);
    this->depthResolvePipeline.Cleanup(device);
    this->postFXPipeline.Cleanup(device);
}

void VulkanRenderer::InitSceneData(gvk::Vulkan& vulkan) {
	this->sceneDataBuffer.Init
    (
        vulkan,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		sizeof(Gltf::GLTFShaderSceneData),
        "Scene buffer"
    );

	this->lightDataBuffer.Init
    (
        vulkan,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		sizeof(Gltf::GLTFLightData),
        "Light buffer"
    );
}

void VulkanRenderer::CreateImages(gvk::Vulkan& vulkan, const glm::ivec2& drawImageSize) {
    const VkExtent3D drawImageExtent {
        .width = (std::uint32_t)drawImageSize.x,
        .height = (std::uint32_t)drawImageSize.y,
        .depth = 1,
    };

    // draw image
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
    
    this->drawImageId = vulkan.GetImageManager().CreateImage(createDrawImageInfo, nullptr, this->drawImageId, "Draw image");

    if (!this->resolveImagesInitialized) {
        // postFX image
        createDrawImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        this->postFXDrawImageId = vulkan.GetImageManager().CreateImage(createDrawImageInfo, nullptr, INVALID_IMAGE_ID, "PostFX image");

        // resolve image
        const CreateImageInfo createResolveDrawImageInfo {
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .usage = usages,
            .extent = drawImageExtent,
        };
        this->resolveDrawImageId = vulkan.GetImageManager().CreateImage(createResolveDrawImageInfo, nullptr, INVALID_IMAGE_ID, "Resolve image");
    }

    // depth image
    CreateImageInfo createDepthImageInfo{
        .format = this->depthImageFormat,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .extent = drawImageExtent,
        .samples = this->samples,
    };

    this->depthImageId = vulkan.GetImageManager().CreateImage(createDepthImageInfo, nullptr, this->depthImageId, "Depth image");

    if (!this->resolveImagesInitialized) {
        createDepthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        this->resolveDepthImageId = vulkan.GetImageManager().CreateImage(createDepthImageInfo, nullptr, INVALID_IMAGE_ID, "Resolve depth image");

        this->resolveImagesInitialized = true;
    }
}

void VulkanRenderer::SortRenderingUnits() {
    this->renderingUnitsOrder.clear();
    this->renderingUnitsOrder.resize(this->renderingUnits.size());
    std::iota(this->renderingUnitsOrder.begin(), this->renderingUnitsOrder.end(), 0);

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

void VulkanRenderer::RenderFrame(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData) {
    LinearColorWithoutAlpha ambient{
        .r = sceneData.ambientColor.r,
        .g = sceneData.ambientColor.g,
        .b = sceneData.ambientColor.b
    };

    LinearColorWithoutAlpha fog{
        .r = sceneData.fogColor.r,
        .g = sceneData.fogColor.g,
        .b = sceneData.fogColor.b
    };

    const Gltf::GLTFShaderSceneData shaderSceneData{
        .view = sceneData.camera.GetView(),
        .projection = sceneData.camera.GetProjection(),
        .viewProjection = sceneData.camera.GetViewProjection(),
        .cameraPos = glm::vec4{ sceneData.camera.GetPosition(), 1.f},
        .ambientColor = ambient,
        .ambientIntensity = sceneData.ambientIntensity,
        .fogColor = fog,
        .fogIntensity = sceneData.fogIntensity,
        .materialsBuffer = this->materialManager.GetMaterialDataBufferAddress(),
    };
    this->sceneDataBuffer.UploadNewFrameData(
        cmdBuffer,
        vulkan.GetCurrentFrame(),
        (void*)&shaderSceneData,
        sizeof(Gltf::GLTFShaderSceneData),
        0,
        true
    );

    const Image& drawImage = vulkan.GetImageManager().GetImage(this->drawImageId);
    const Image& resolveImage = vulkan.GetImageManager().GetImage(this->resolveDrawImageId);
    const Image& depthImage = vulkan.GetImageManager().GetImage(this->depthImageId);

    {
        Debug::BeginDebugLabel(cmdBuffer, "Geometry + Skybox");
        // geometry rendering
        Util::PipelineImageTransition(
            cmdBuffer,
            drawImage.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        );
        Util::PipelineImageTransition(
            cmdBuffer,
            depthImage.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
        );
        if (this->MultisamplingEnabled()) {
            Util::PipelineImageTransition(
                cmdBuffer,
                resolveImage.image,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            );
        }

        const RenderInfo meshRenderInfo = StructCreators::CreateRenderingInfo({
            .extent = drawImage.getExtent2D(),
            .colorImageView = drawImage.imageView,
            .colorImageClearValue = glm::vec4{0.f, 0.f, 0.f, 1.f},
            .depthImageView = depthImage.imageView,
            .depthImageClearValue = 0.f,
            .resolveImageView = this->MultisamplingEnabled() ? resolveImage.imageView : VK_NULL_HANDLE,
            //.resolveImageView = VK_NULL_HANDLE
        });

        vkCmdBeginRendering(cmdBuffer, &meshRenderInfo.renderingInfo);

        meshPipeline.Draw(
            cmdBuffer,
            drawImage.getExtent2D(),
            vulkan,
            this->meshManager,
            this->materialManager,
            sceneData.camera,
            sceneDataBuffer.GetBuffer(),
            this->renderingUnits,
            this->renderingUnitsOrder
        );
        skyboxPipeline.Draw(cmdBuffer, vulkan, sceneData.camera);
        vkCmdEndRendering(cmdBuffer);
        Debug::EndBeginLabel(cmdBuffer);
    }

    //// Synchronization with next frame ////
    const VkImageMemoryBarrier2 imageBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        //.image = drawImage.image,
        .image = this->MultisamplingEnabled() ? resolveImage.image : drawImage.image,
        .subresourceRange = StructCreators::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT),
    };
    const VkImageMemoryBarrier2 depthBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .image = depthImage.image,
        .subresourceRange = StructCreators::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT),
    };
    const VkImageMemoryBarrier2 barriers[2] {imageBarrier, depthBarrier};
    const auto dependencyInfo = VkDependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 2,
        .pImageMemoryBarriers = barriers,
    };
    vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
    ////////


    // multisampling rendering
    if (this->MultisamplingEnabled()) {
        Debug::BeginDebugLabel(cmdBuffer, "Depth Resolve");

        const Image& resolveDepthImage = vulkan.GetImageManager().GetImage(this->resolveDepthImageId);
        Util::PipelineImageTransition(
            cmdBuffer,
            resolveDepthImage.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        const RenderInfo resolveRenderInfo = StructCreators::CreateRenderingInfo({
            .extent = resolveDepthImage.getExtent2D(),
            .depthImageView = resolveDepthImage.imageView
        });

        vkCmdBeginRendering(cmdBuffer, &resolveRenderInfo.renderingInfo);

        this->depthResolvePipeline.Draw(cmdBuffer, vulkan, depthImage, this->SamplesToInt(this->samples));

        vkCmdEndRendering(cmdBuffer);

        Util::PipelineImageTransition(
            cmdBuffer,
            resolveDepthImage.image,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        Debug::EndBeginLabel(cmdBuffer);
    }

    // post FX rendering
    const Image& postFXDrawImage = vulkan.GetImageManager().GetImage(this->postFXDrawImageId);

    Debug::BeginDebugLabel(cmdBuffer, "Post FX");
    Util::PipelineImageTransition
    (
        cmdBuffer,
        postFXDrawImage.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    const RenderInfo postFXRenderInfo = StructCreators::CreateRenderingInfo({
        .extent = postFXDrawImage.getExtent2D(),
        .colorImageView = postFXDrawImage.imageView
    });
    vkCmdBeginRendering(cmdBuffer, &postFXRenderInfo.renderingInfo);
    if (this->MultisamplingEnabled()) {
        const Image& resolveDepthImage = vulkan.GetImageManager().GetImage(this->resolveDepthImageId);
        this->postFXPipeline.Draw
        (
            cmdBuffer,
            vulkan,
            resolveImage,
            resolveDepthImage,
            this->sceneDataBuffer.GetBuffer()
        );
    }
    else 
        this->postFXPipeline.Draw
        (
            cmdBuffer,
            vulkan,
            drawImage,
            depthImage,
            this->sceneDataBuffer.GetBuffer()
        );

    vkCmdEndRendering(cmdBuffer);

    Debug::EndBeginLabel(cmdBuffer);
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

void VulkanRenderer::SetSkyboxImage(const ImageId skyboxId) {
    this->skyboxPipeline.SetSkyboxTexture(skyboxId);
}

int VulkanRenderer::SamplesToInt(VkSampleCountFlagBits samples)
{
    switch (samples) {
    case VK_SAMPLE_COUNT_1_BIT:
        return 1;
    case VK_SAMPLE_COUNT_2_BIT:
        return 2;
    case VK_SAMPLE_COUNT_4_BIT:
        return 4;
    case VK_SAMPLE_COUNT_8_BIT:
        return 8;
    case VK_SAMPLE_COUNT_16_BIT:
        return 16;
    case VK_SAMPLE_COUNT_32_BIT:
        return 32;
    case VK_SAMPLE_COUNT_64_BIT:
        return 64;
    default:
        return 0;
    }
}