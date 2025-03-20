#pragma once

#include <SDL.h>

#include "Engine/EngineTypes.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/MeshManager.h"
#include "Renderer/Vulkan/MaterialManager.h"

class Engine {
	WindowParams wParams;
	SDL_Window* window;

	gvk::Vulkan vulkan;
	MeshManager meshManager;
	MaterialManager materialManager;

public:
	Engine(const WindowParams& params);
	void run();
	~Engine();
};