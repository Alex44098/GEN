#pragma once

#include "GECSHeaders.h"

struct EngineConfig {
	std::string win_name { "GEN" };
	GECS::i32 win_width { 480 };
	GECS::i32 win_height { 360 };
	bool use_a2c{ false };

	std::string scene_name;
};