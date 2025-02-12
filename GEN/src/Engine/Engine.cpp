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

	GECS::Init();

	// there should be a creation of engine systems here
	// ...
	//
}

void Engine::run() {
	const float FPS = 60.f;
	const float dt = 1.f / FPS;

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
	GECS::Destroy();

	SDL_DestroyWindow(this->window);
	SDL_Quit();
}