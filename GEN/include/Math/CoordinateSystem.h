#pragma once

#include <glm.hpp>

// Right-handed coordinate system
// +X - left
// +Y - up
// +Z - forward

// !!!WARNING!!!
// But glTF 2.0 uses a right-handed coordinate system.
// glTF defines +Y as up, +Z as forward, and -X as right;
// the front of a glTF asset faces +Z.

namespace CoordSystem {
	inline constexpr glm::vec3 upAxis{ 0.f, 1.f, 0.f };
	inline constexpr glm::vec3 downAxis{ 0.f, -1.f, 0.f };

	inline constexpr glm::vec3 rightAxis{ -1.f, 0.f, 0.f };
	inline constexpr glm::vec3 leftAxis{ 1.f, 0.f, 0.f };

	inline constexpr glm::vec3 frontAxis{ 0.f, 0.f, 1.f };
	inline constexpr glm::vec3 backAxis{ 0.f, 0.f, -1.f };
}
