#include "Engine/Engine.h"

Engine::Engine(const WindowParams& params) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	this->wParams = params;
	this->window = SDL_CreateWindow(
		this->wParams.name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		this->wParams.size.x,
		this->wParams.size.y,
		SDL_WINDOW_VULKAN);

	SDL_SetWindowResizable(this->window, SDL_TRUE);

	if (!window) {
		assert(false && "SDL window not created");
	}

	this->vulkan.Init(this->window);
	this->materialManager.Init(this->vulkan);

	GECS::Init();
	// there should be a creation of engine systems here
	// ...
	//
	this->sceneSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<SceneSystem>(this->vulkan, this->meshManager, this->materialManager);
	this->sceneSystem->LoadScene("scenes/SimpleBox/scene.gltf");

	this->renderSystem = GECS::GECSInstance->GetSystemManager()->AddSystem<RenderSystem>(this->vulkan, this->meshManager, this->materialManager, this->wParams.size);
}

void Engine::run() {
	const GECS::f32 FPS = 60.f;
	const GECS::f32 dt = 1.f / FPS;

	bool running = true;
	SDL_Event SDLEvents;

	while (running) {
		while (SDL_PollEvent(&SDLEvents)) {
			if (SDLEvents.type == SDL_QUIT) {
				running = false;
				return;
			}
			if (SDLEvents.type == SDL_WINDOWEVENT) {
				switch (SDLEvents.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					this->wParams.size = { SDLEvents.window.data1, SDLEvents.window.data2 };
					break;
				}
			}
		}

		if (this->vulkan.SwapchainNeedsRecreation()) {
			this->vulkan.RecreateSwapchain(wParams.size.x, wParams.size.y);
		}

		GECS::GECSInstance->Update(dt);
	}
}

Engine::~Engine() {
	this->vulkan.WaitIdle();

	GECS::Destroy();

	this->meshManager.CleanMeshes(this->vulkan);
	this->materialManager.Destroy(this->vulkan);
	this->vulkan.Destroy();

	SDL_DestroyWindow(this->window);
	SDL_Quit();
}