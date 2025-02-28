#pragma once

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/Material.h"
#include "Renderer/Vulkan/Buffer.h"
#include "Renderer/RenderTypes.h"

#define MAX_NUM_MATERIALS 1000

struct MaterialData {
	LinearColor baseColor;
	std::uint32_t diffuseTexture;
	std::uint32_t normalTexture;
};

class MaterialManager {
	Buffer materialDataBuffer; // one buffer for all materials
	std::vector<Material> materials;

	MaterialId placeholderMaterialId;

	ImageId placeholderDiffuseTex;
	ImageId placeholderNormalTex;

public:
	MaterialManager(gvk::Vulkan& vulkan);
	void Destroy(gvk::Vulkan& vulkan);

	MaterialId AddMaterial(gvk::Vulkan& vulkan, Material material);
	const Material& GetMaterial(MaterialId id) const;

	inline const Buffer& GetMaterialDataBuffer() const { return this->materialDataBuffer; }
	inline VkDeviceAddress GetMaterialDataBufferAddress() const{ return this->materialDataBuffer.address; }

private:
	void CreatePlaceholders(gvk::Vulkan& vulkan);
};