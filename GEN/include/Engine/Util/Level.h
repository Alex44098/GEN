#pragma once

#include <filesystem>
#include <glm.hpp>

#include "GECSHeaders.h"

struct Level {
	std::filesystem::path scenePath;
	std::filesystem::path skyboxPath;

	glm::vec3 ambientColor { 0, 0, 0 };
	GECS::f32 ambientIntensity { 0 };
	glm::vec3 fogColor { 0, 0, 0 };
	GECS::f32 fogIntensity { 0 };
};