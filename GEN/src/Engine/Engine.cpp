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
	this->sceneSystem->LoadScene("scenes/City/City.gltf");
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
		}

		GECS::GECSInstance->Update(dt);
	}
}

Engine::~Engine() {
	this->meshManager.CleanMeshes(this->vulkan);
	this->materialManager.Destroy(this->vulkan);
	this->vulkan.Destroy();

	GECS::Destroy();

	SDL_DestroyWindow(this->window);
	SDL_Quit();
}