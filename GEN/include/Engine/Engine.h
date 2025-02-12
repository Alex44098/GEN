#pragma once

#include <SDL.h>

#include "Engine/EngineTypes.h"

class Engine {
	WindowParams wParams;
	SDL_Window* window;

public:
	Engine(const WindowParams& params);
	void run();
	~Engine();
};