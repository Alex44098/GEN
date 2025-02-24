#pragma once

#include "Engine/EngineTypes.h"

struct Material {
	ImageId diffuseTexture;
	ImageId normalMapTexture;

	std::string name;
};