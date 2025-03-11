#include "Renderer/Gltf/MaterialLoader.h"

namespace Gltf {
	Material LoadMaterial(gvk::Vulkan& vulkan, const tinygltf::Model& model, const std::filesystem::path& fileDir, const tinygltf::Material& gltfMaterial) {
		Material material{
			.baseColor = GetDiffuseColor(gltfMaterial),
			.name = gltfMaterial.name
		};

		if (HasDiffuseTexture(gltfMaterial)) {
			const std::filesystem::path diffusePath = GetDiffuseTexturePath(model, gltfMaterial, fileDir);
			material.diffuseTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				diffusePath,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);
		}

		if (HasNormalMapTexture(gltfMaterial)) {
			const std::filesystem::path normalMapPath = GetNormalMapTexturePath(model, gltfMaterial, fileDir);
			material.normalMapTexture = vulkan.GetImageManager().LoadImageFromFile
			(
				normalMapPath,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				true
			);
		}

		return material;
	}

	LinearColor GetDiffuseColor(const tinygltf::Material& gltfMaterial) {
		const std::vector<double> color = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
		assert(color.size() == 4);
		return LinearColor{ (float)color[0], (float)color[1], (float)color[2], (float)color[3] };
	}

	std::filesystem::path GetDiffuseTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	std::filesystem::path GetNormalMapTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir) {
		const GECS::i32 textureIndex = gltfMaterial.normalTexture.index;
		const tinygltf::Texture& texture = model.textures[textureIndex];
		const tinygltf::Image& image = model.images[texture.source];

		return fileDir / image.uri;
	}

	bool HasDiffuseTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
		return textureIndex != -1;
	}

	bool HasNormalMapTexture(const tinygltf::Material& gltfMaterial) {
		const GECS::i32 textureIndex = gltfMaterial.normalTexture.index;
		return textureIndex != -1;
	}
}