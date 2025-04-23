#pragma once

#include <filesystem>

#include "GECSHeaders.h"
#include "Engine/EngineTypes.h"

struct Level {
	std::filesystem::path scenePath;
	std::filesystem::path skyboxPath;

	LinearColor ambientColor { 0, 0, 0 };
	GECS::f32 ambientIntensity { 0 };
	LinearColor fogColor { 0, 0, 0 };
	GECS::f32 fogIntensity { 0 };

	glm::vec3 cameraPos{ 0, 0, 0 };
};