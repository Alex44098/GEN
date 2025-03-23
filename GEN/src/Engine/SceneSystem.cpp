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
		return;
	}

	const Gltf::GLTFScene& scene = this->sceneManager.LoadScene(path);
	
	std::cout << scene.nodes.size() << std::endl;

	this->GenerateEntities(scene);
}

void SceneSystem::GenerateEntities(const Gltf::GLTFScene& scene) {
	for (const std::unique_ptr<Gltf::GLTFSceneNode>& rootNode : scene.nodes) {
		this->ProcessNodeScene(scene, *rootNode.get());
	}
}

void SceneSystem::ProcessNodeScene(const Gltf::GLTFScene& scene, const Gltf::GLTFSceneNode& node) {
	if (node.meshIndex != -1) {
		this->CreateEntity(scene.sceneMeshes[node.meshIndex], node.transform.GetMatrix());
	}
	for (const std::unique_ptr<Gltf::GLTFSceneNode>& childNode : node.children)
		this->ProcessNodeScene(scene, *childNode.get());
}

void SceneSystem::CreateEntity(const Gltf::GLTFSceneMesh& sceneMesh, const glm::mat4 transform) {
	GECS::Handle entityHandle = GECS::GECSInstance->GetEntityManager()->CreateEntity<RenderableGameObject>();

	RenderComponent* rc = GECS::GECSInstance->GetEntityManager()->GetEntity(entityHandle)->GetComponent<RenderComponent>();

	for (std::size_t i = 0; i < sceneMesh.primitives.size(); i++) {
		rc->meshes.push_back(sceneMesh.primitives[i]);
		rc->materials.push_back(sceneMesh.primitiveMaterials[i]);
		rc->transforms.push_back(transform);
	}
}