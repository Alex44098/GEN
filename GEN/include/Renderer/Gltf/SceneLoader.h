#pragma once

// About mesh: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#geometry
// About material: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#materials

#include <filesystem>
#include <span>

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"
#include "Math/Transform.h"

#include "Renderer/Gltf/GLTFScene.h"
#include "Renderer/Gltf/GLTFLight.h"

#define GLTF_LIGHTS_PUNCTUAL_KEY "KHR_lights_punctual"
#define GLTF_LIGHTS_PUNCTUAL_POINT_NAME "point"
#define GLTF_LIGHTS_PUNCTUAL_DIRECTIONAL_NAME "directional"
#define GLTF_LIGHTS_PUNCTUAL_SPOT_NAME "spot"

#define GLTF_POSITIONS_ACCESSOR "POSITION"
#define GLTF_NORMALS_ACCESSOR "NORMAL"
#define GLTF_TANGENTS_ACCESSOR "TANGENT"
#define GLTF_TEXCOORD_ACCESSOR "TEXCOORD_0"

namespace tinygltf {
	class Model;
	class Node;
	struct Primitive;
	struct Material;
	struct Light;
}

namespace Gltf {
	// Mesh loading
	GECS::i32 GetAccessorIndex(const tinygltf::Primitive& primitive, const std::string& name);
	Mesh CreateMeshFromPrimitive(const tinygltf::Model& model, const std::string& name, const tinygltf::Primitive& primitive);

	// Material loading
	Material LoadMaterial(gvk::Vulkan& vulkan, const tinygltf::Model& model, const std::filesystem::path& path, const tinygltf::Material& gltfMaterial);
	LinearColor GetDiffuseColor(const tinygltf::Material& gltfMaterial);
	std::filesystem::path GetDiffuseTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir);
	std::filesystem::path GetNormalMapTexturePath(const tinygltf::Model& model, const tinygltf::Material& gltfMaterial, const std::filesystem::path& fileDir);
	bool HasDiffuseTexture(const tinygltf::Material& gltfMaterial);
	bool HasNormalMapTexture(const tinygltf::Material& gltfMaterial);

	// Light loading
	LinearColor Tg2LinearColor(const std::vector<double>& vec);
	GLTFLightType FromGLTF2builtInLight(const std::string& lightType);
	GLTFLight LoadLight(const tinygltf::Light& light);

	void BuildNodeTree(GLTFSceneNode& node, const tinygltf::Node& gltfNode, const tinygltf::Model& model);

	GLTFScene LoadGLTFScene(const std::filesystem::path& path, gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager);
}