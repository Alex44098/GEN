#pragma once

#include <array>
#include <glm.hpp>

struct Camera;

namespace Primitives {
	struct AABB;
	struct Sphere;
	struct Plane;
}

namespace FrustumCulling
{
	struct Frustum {
		std::array<Primitives::Plane, 6> planes;
	};

	// Calculates the 8 corners of a truncated pyramid in world coordinates.
	std::array<glm::vec3, 8> calculateFrustumCornersWorldSpace(const Camera& camera);

	Frustum createFrustumFromCamera(const Camera& camera);
	bool isInFrustum(const Frustum& frustum, const Primitives::Sphere& s);
	bool isInFrustum(const Frustum& frustum, const Primitives::AABB& aabb);

	Primitives::Sphere calculateBoundingSphereWorld(
		const glm::mat4& transform,
		const Primitives::Sphere& s);
}