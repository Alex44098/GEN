#include "Renderer/Vulkan/MaterialManager.h"
#include "Renderer/Vulkan/Util/DebugLabels.h"

void MaterialManager::Init(gvk::Vulkan& vulkan) {
	this->materialDataBuffer = vulkan.CreateBuffer(MAX_NUM_MATERIALS * sizeof(MaterialData),
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_AUTO);

	Material placeholderMaterial;
	placeholderMaterial.name = "placeholderMaterial";
	this->placeholderMaterialId = this->AddMaterial(vulkan, placeholderMaterial);

	this->CreatePlaceholders(vulkan);

	Debug::AddDebugLabel4Buffer(vulkan.GetDevice(), this->materialDataBuffer.vkBuffer, "Material buffer");
}

void MaterialManager::Destroy(gvk::Vulkan& vulkan) {
	vulkan.DestroyBuffer(this->materialDataBuffer);
}

void MaterialManager::CreatePlaceholders(gvk::Vulkan& vulkan) {
	// create normal map texture placeholder
	GECS::u32 normalMap = 0xFFFF8080;
	this->placeholderNormalTex = vulkan.GetImageManager().CreateImage(
		{
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.extent = VkExtent3D{1, 1, 1}
		},
		&normalMap,
		INVALID_IMAGE_ID,
		"Placeholder normal"
	);

	// create diffuse texture placeholder
	const GECS::u32 black = 0xFF000000;
	const GECS::u32 magenta = 0xFFFF00FF;
	//GECS::u32 textureData[4] { black, magenta, black, magenta };
	GECS::u32 textureData[4]{ black, black, black, black };
	this->placeholderDiffuseTex = vulkan.GetImageManager().CreateImage(
		{
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			.extent = VkExtent3D{ 2, 2, 1 }
		},
		textureData,
		INVALID_IMAGE_ID,
		"Placeholder diffuse"
	);
}

MaterialId MaterialManager::AddMaterial(gvk::Vulkan& vulkan, Material material) {
	MaterialData* matData = (MaterialData*)materialDataBuffer.allocInfo.pMappedData;
	GECS::u32 id = this->materials.size();
	matData[id] = MaterialData{
		.baseColor = material.baseColor,

		.factors = glm::vec4{
			material.metallicFactor,
			material.roughnessFactor,
			material.emissiveFactor,
			0.f
		},

		.diffuseTexture = material.diffuseTexture != INVALID_IMAGE_ID
			? material.diffuseTexture : this->placeholderDiffuseTex,
		.normalTexture = material.normalMapTexture != INVALID_IMAGE_ID
			? material.normalMapTexture : this->placeholderNormalTex,
		.metallicRoughTexture = material.metallicRoughnessTexture != INVALID_IMAGE_ID
			? material.metallicRoughnessTexture : this->placeholderDiffuseTex,
		.emissiveTexture = material.emissiveTexture != INVALID_IMAGE_ID
			? material.emissiveTexture : this->placeholderDiffuseTex
	};

	materials.push_back(std::move(material));
	return id;
}

const Material& MaterialManager::GetMaterial(MaterialId id) const {
	return materials.at(id);
}