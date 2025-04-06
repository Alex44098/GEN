#pragma once

#include "GECSHeaders.h"

struct MouseMoveEvent : public GECS::Event::Event<MouseMoveEvent> {
	GECS::i32 mouseRelX, mouseRelY;

	MouseMoveEvent(GECS::f32 relx, GECS::f32 rely) : mouseRelX(relx), mouseRelY(rely) { }
};