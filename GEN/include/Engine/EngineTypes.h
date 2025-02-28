#pragma once

#include <GECSHeaders.h>
#include <vec2.hpp>
#include <string>

using GameObjectId = GECS::Handle;

struct WindowParams {
	glm::ivec2 size;
	std::string name;
};