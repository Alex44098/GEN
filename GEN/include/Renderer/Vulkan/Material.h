#pragma once

#include "Renderer/RenderTypes.h"

struct Material {
	LinearColor baseColor;
	GECS::f32 metallicFactor{0.f};
	GECS::f32 roughnessFactor{0.5f};
	GECS::f32 emissiveFactor{0.f};

	ImageId diffuseTexture{ INVALID_IMAGE_ID };
	ImageId normalMapTexture{ INVALID_IMAGE_ID };
	ImageId metallicRoughnessTexture{ INVALID_IMAGE_ID };
	ImageId emissiveTexture{ INVALID_IMAGE_ID };

	std::string name;
};