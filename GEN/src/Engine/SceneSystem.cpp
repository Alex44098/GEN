#include "Engine/SceneSystem.h"
#include <iostream>

SceneSystem::SceneSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager)
	: vulkan(vulkan), meshManager(meshManager), materialManager(materialManager),
		sceneManager(vulkan, meshManager, materialManager)
{}

SceneSystem::~SceneSystem() {}

void SceneSystem::LoadScene(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path))
		assert(false && "Scene system: scene was not found");

	const Gltf::GLTFScene& scene = this->sceneManager.LoadScene(path);
	
	std::cout << scene.nodes.size() << std::endl;
}

void SceneSystem::GenerateEntities(const Gltf::GLTFScene& scene) {
	for (const std::unique_ptr<Gltf::GLTFSceneNode>& rootNodes : scene.nodes) {

	}
}