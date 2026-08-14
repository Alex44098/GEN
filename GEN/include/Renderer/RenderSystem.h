#pragma once

#include "GECSHeaders.h"
#include "ComponentManager.h"

#include "Components/RenderComponent.h"
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"
#include "Renderer/Gltf/SceneManager.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "Renderer/Camera.h"

#include "Engine/Util/Level.h"
#include "Engine/EngineConfig.h"

class RenderSystem : public GECS::System<RenderSystem> {
	gvk::Vulkan& vulkan;
	MeshManager& meshManager;
	MaterialManager& materialManager;

	VulkanRenderer renderer;
	Camera camera;

	bool drawCullingLines = false;

	glm::ivec2 windowPos{};
	glm::ivec2 windowSize{};

	const Level currentLevel;

public:
	RenderSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager, const EngineConfig config, const Level level);
	~RenderSystem();

	virtual void Update(GECS::f32 delta) override;

	Camera& GetCamera() { return this->camera; }

private:
	void CreateDraws(gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData);
	void CollectRenderingUnits();
	
#ifdef _DEBUG
	void CollectDebugDraws(gvk::Vulkan& vulkan, const Gltf::GLTFSceneData& sceneData);
#endif

	glm::vec4 CalcLetterBox(const glm::ivec2 srcSize, const glm::ivec2 dstSize, GECS::i32 scale);
	void ProcessKeyDown(const GECS::Event::IEvent* e);
};