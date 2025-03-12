#pragma once

#include <filesystem>

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Gltf/GLTFScene.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"

class SceneManager {
	std::unordered_map < std::string, Gltf::GLTFScene> scenes;

	gvk::Vulkan& vulkan;
	MeshManager& meshManager;
	MaterialManager& materialManager;

public:
	SceneManager(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager);

	const Gltf::GLTFScene& AddScene(const std::string& path, Gltf::GLTFScene scene);
	const Gltf::GLTFScene& GetScene(const std::string& path) const;
	const Gltf::GLTFScene& LoadScene(const std::filesystem::path& path);
};