#pragma once

#include "Frustum.h"

#include <glm.hpp>

namespace Primitives {
	struct AABB;
	struct Sphere;
}

namespace FrustumCulling {
	bool IsInFrustum(const Frustum& frustum, const Primitives::Sphere& s);
	bool IsInFrustum(const Frustum& frustum, const Primitives::AABB& aabb);

	Primitives::Sphere CalculateBoundingSphereWorld(
		const glm::mat4& transform,
		const Primitives::Sphere& s);
}