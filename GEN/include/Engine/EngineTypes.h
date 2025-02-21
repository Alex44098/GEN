#pragma once

#include <GECSHeaders.h>
#include <vec2.hpp>
#include <string>

#define MAX_FRAMES_IN_FLIGHT 2

struct WindowParams {
	glm::ivec2 size;
	std::string name;
};

using GameObjectId = GECS::Handle;

