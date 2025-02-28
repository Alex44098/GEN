#pragma once

#include "Renderer/RenderTypes.h"

struct Material {
	LinearColor baseColor;

	ImageId diffuseTexture;
	ImageId normalMapTexture;

	std::string name;
};