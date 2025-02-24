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

using MeshId = GECS::u32;
using MaterialId = GECS::u32;
using ImageId = GECS::u32;