#pragma once

#include <GECSHeaders.h>
#include <vector>
#include <filesystem>

#include "Renderer/Gltf/GLTFScene.h"
#include "Renderer/Gltf/SceneManager.h"
#include "Math/Transform.h"
#include "GameObject/RenderableGameObject.h"
#include "Components/RenderComponent.h"

class SceneSystem : public GECS::System<SceneSystem> {
	gvk::Vulkan& vulkan;
	MeshManager& meshManager;
	MaterialManager& materialManager;

	SceneManager sceneManager;

public:
	SceneSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager);
	~SceneSystem();

	void LoadScene(const std::filesystem::path& path);

private:
	void GenerateEntities(const Gltf::GLTFScene& scene);
	void ProcessNodeScene(const Gltf::GLTFScene& scene, const Gltf::GLTFSceneNode& node);
	void CreateEntity(const Gltf::GLTFSceneMesh& sceneMesh, const glm::mat4 transform);
};