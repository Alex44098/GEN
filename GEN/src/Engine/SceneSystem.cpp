#include "Engine/SceneSystem.h"
#include <iostream>

SceneSystem::SceneSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager)
	: vulkan(vulkan), meshManager(meshManager), materialManager(materialManager),
		sceneManager(vulkan, meshManager, materialManager)
{
	std::cout << "Scene manager initialized!" << std::endl;
}

SceneSystem::~SceneSystem() {}

void SceneSystem::LoadScene(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		std::cout << "Scene not found!" << std::endl;
		//assert(false && "Scene system: scene was not found");
		return;
	}

	const Gltf::GLTFScene& scene = this->sceneManager.LoadScene(path);
	
	std::cout << scene.nodes.size() << std::endl;
}

void SceneSystem::GenerateEntities(const Gltf::GLTFScene& scene) {
	for (const std::unique_ptr<Gltf::GLTFSceneNode>& rootNodes : scene.nodes) {

	}
}