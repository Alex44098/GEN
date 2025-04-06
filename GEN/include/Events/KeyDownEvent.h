#pragma once

#include <SDL.h>
#include "GECSHeaders.h"

struct KeyDownEvent : public GECS::Event::Event<KeyDownEvent> {
	SDL_Keycode keyCode;

	KeyDownEvent(SDL_Keycode keyCode) : keyCode(keyCode) {}
};