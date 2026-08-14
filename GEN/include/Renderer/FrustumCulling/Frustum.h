#pragma once

#include "Math/GeometricPrimitive/Plane.h"

#include <array>
#include <glm.hpp>

class Camera;

namespace FrustumCulling
{
	struct Frustum {
		std::array<Primitives::Plane, 6> planes;
	};

	// Calculates the 8 corners of a truncated pyramid in world coordinates.
	std::array<glm::vec3, 8> CalculateFrustumCornersWorldSpace(const Camera& camera);

	Frustum CreateFrustumFromCamera(const Camera& camera);
}