#pragma once

// See more: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#scenes

#include <filesystem>

#include "Renderer/Gltf/GLTFLight.h"
#include "Renderer/Vulkan/Mesh.h"
#include "Renderer/RenderTypes.h"
#include "Math/Transform.h"

struct GLTFSceneNode {
	std::string name;
	Transform transform;
	GECS::i32 meshIndex{ -1 };
	GECS::i32 lightIndex{ -1 };

	GLTFSceneNode* parent{ nullptr };
	std::vector<GLTFSceneNode*> children;
};

struct GLTFSceneMesh {
	std::vector<MeshId> primitives;
	std::vector<MaterialId> primitiveMaterials;
};

struct GLTFScene {
	std::filesystem::path path;

	// root nodes
	std::vector<GLTFSceneNode*> nodes;

	std::vector<GLTFSceneMesh> meshes;
	std::vector<GLTFLight> lights;
	std::unordered_map<MeshId, Mesh*> meshes;
};