#include "Renderer/Gltf/SceneManager.h"
#include "Renderer/Gltf/SceneLoader.h"

SceneManager::SceneManager(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager) :
	vulkan(vulkan), meshManager(meshManager), materialManager(materialManager)
{}

const Gltf::GLTFScene& SceneManager::AddScene(const std::string& path, Gltf::GLTFScene scene) {
	scene.path = path;
	const auto [pair, inserted] = this->scenes.emplace(path, std::move(scene));
	assert(inserted && "Scene manager: failed to add scene");

	return pair->second;
}

const Gltf::GLTFScene& SceneManager::GetScene(const std::string& path) const {
	const auto pathAndScene = this->scenes.find(path);
	if (pathAndScene == scenes.end())
		assert(false && "Scene manager: scene was not loaded");

	return pathAndScene->second;
}

const Gltf::GLTFScene& SceneManager::LoadScene(const std::filesystem::path& path) {
	const auto pathAndScene = this->scenes.find(path.string());
	if (pathAndScene != this->scenes.end())
		return pathAndScene->second;

	Gltf::GLTFScene scene = Gltf::LoadGLTFScene(path, this->vulkan, this->meshManager, this->materialManager);
	const auto [pair, inserted] = this->scenes.emplace(path.string(), std::move(scene));
	assert(inserted && "Scene manager: failed to load scene");

	return pair->second;
}