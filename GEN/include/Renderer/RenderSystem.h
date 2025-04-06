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

class RenderSystem : public GECS::System<RenderSystem> {
	gvk::Vulkan& vulkan;
	MeshManager& meshManager;
	MaterialManager& materialManager;

	VulkanRenderer renderer;
	Camera camera;

	glm::ivec2 windowPos;
	glm::ivec2 windowSize;

	bool sceneLoaded{ false };
public:
	RenderSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& materialManager, const glm::ivec2& drawImageSize);
	~RenderSystem();

	virtual void Update(GECS::f32 delta) override;

	Camera& GetCamera() { return this->camera; }

private:
	void CreateDraws();

	glm::vec4 CalcLetterBox(const glm::ivec2 srcSize, const glm::ivec2 dstSize, GECS::i32 scale);
};