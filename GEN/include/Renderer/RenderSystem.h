#pragma once

#include "GECSHeaders.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"
#include "Renderer/Gltf/SceneManager.h"

class RenderSystem : GECS::System<RenderSystem> {
	gvk::Vulkan& vulkan;
	MeshManager& meshManager;
	MaterialManager& materialManager;

	bool sceneLoaded{ false };
public:
	RenderSystem(gvk::Vulkan& vulkan, MeshManager& meshManager, MaterialManager& meterialManager);
	~RenderSystem();

	virtual void Update(GECS::f32 delta) override;

private:

};