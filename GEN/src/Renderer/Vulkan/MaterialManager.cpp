#include "Renderer/Vulkan/MaterialManager.h"

MaterialManager::MaterialManager(gvk::Vulkan& vulkan) {
	this->materialDataBuffer = vulkan.CreateBuffer(MAX_NUM_MATERIALS * sizeof(MaterialData),
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_AUTO);

	Material placeholderMaterial;
	placeholderMaterial.name = "placeholderMaterial";
	this->placeholderMaterialId = this->AddMaterial(vulkan, placeholderMaterial);
}

void MaterialManager::Destroy(gvk::Vulkan& vulkan) {
	vulkan.DestroyBuffer(this->materialDataBuffer);
}

//MaterialId MaterialManager::AddMaterial(gvk::Vulkan& vulkan, Material material) {
//	MaterialData* matData = (MaterialData*)materialDataBuffer.allocInfo.pMappedData;
//
//}