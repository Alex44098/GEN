#pragma once

#include "Renderer/Vulkan/Pipelines/Pipeline.h"
#include "Renderer/Camera.h"
#include "Renderer/Vulkan/Util/ShaderModuleLoader.h"

class SkyboxPipeline : public Pipeline {
	struct Constants {
		glm::mat4 invViewProj;
		glm::vec3 cameraPos;
		std::uint32_t skyboxTextureId;
	};

	ImageId skyboxTextureId { INVALID_IMAGE_ID };

public:
	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) override;
	virtual void Cleanup(VkDevice device) override;

	void Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Camera& camera);

	void SetSkyboxTexture(const ImageId skyboxId);
};