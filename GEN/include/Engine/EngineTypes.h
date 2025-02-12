#pragma once

#include <GECSHeaders.h>
#include <vec2.hpp>
#include <string>

struct WindowParams {
	glm::ivec2 size;
	std::string name;
};

using GameObjectId = GECS::Handle;

