#pragma once

#include <GECSHeaders.h>

#define MAX_FRAMES_IN_FLIGHT 2

#define INVALID_MESH_ID UINT32_MAX
#define INVALID_MATERIAL_ID UINT32_MAX
#define INVALID_BINDLESS_ID UINT32_MAX

using MeshId = GECS::u32;
using MaterialId = GECS::u32;
using ImageId = GECS::u32;
using BindlessId = GECS::u32;

struct RGBColor {
	GECS::u8 r{ 0 };
	GECS::u8 g{ 0 };
	GECS::u8 b{ 0 };
	GECS::u8 a{ 255 };
};

struct LinearColor {
	float r{ 0.f };
	float g{ 0.f };
	float b{ 0.f };
	float a{ 1.f };
};

struct LinearColorWithoutAlpha {
	float r{ 0.f };
	float g{ 0.f };
	float b{ 0.f };
};