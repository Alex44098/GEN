#include "Engine/InputSystem.h"

#include "Events/MouseMoveEvent.h"
#include "Events/KeyDownEvent.h"

InputSystem::InputSystem() {
	const GECS::u8* keyStates = SDL_GetKeyboardState(&(this->numKeys));

	this->oldKeyStates = new unsigned char[this->numKeys];
	memcpy(this->oldKeyStates, keyStates, sizeof(unsigned char) * this->numKeys);
}

InputSystem::~InputSystem() {
	delete[] this->oldKeyStates;
	this->oldKeyStates = nullptr;
}

void InputSystem::Update(GECS::f32 delta) {
	SDL_Event event;

	while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_MOUSEWHEEL)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			GECS::GECSInstance->GetEventQueue()->Send<KeyDownEvent>
				(
					event.key.keysym.sym
				);
			break;

		case SDL_MOUSEMOTION:
			GECS::GECSInstance->GetEventQueue()->Send<MouseMoveEvent>
				(
					static_cast<GECS::f32>(event.motion.xrel),
					static_cast<GECS::f32>(event.motion.yrel)
				);
			break;
		}
	}

	memcpy(this->oldKeyStates, SDL_GetKeyboardState(nullptr), sizeof(unsigned char) * this->numKeys);
}