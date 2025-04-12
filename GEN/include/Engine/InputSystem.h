#pragma once

#include <GECSHeaders.h>
#include <SDL.h>

class InputSystem : public GECS::System<InputSystem> {
	GECS::i32 numKeys;
	unsigned char* oldKeyStates;

public:
	InputSystem();
	~InputSystem();

	virtual void Update(GECS::f32) override;
};