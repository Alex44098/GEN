#pragma once

#include <SDL.h>

#include "Engine/EngineConfig.h"
#include "Engine/EngineTypes.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"

#include "Engine/InputSystem.h"
#include "Engine/SceneSystem.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/CameraSystem.h"

#include "Engine/Util/Level.h"

class Engine {
	EngineConfig config;
	SDL_Window* window;

	gvk::Vulkan vulkan;
	MeshManager meshManager;
	MaterialManager materialManager;

	SceneSystem* sceneSystem;
	RenderSystem* renderSystem;

public:
	Engine(const char* configPath);
	void run();
	~Engine();

private:
	const Level LoadLevel(std::string file);
	const EngineConfig LoadConfig(const char* configPath);
};