#pragma once

#include <glm.hpp>

#include "Math/GeometricPrimitive/Sphere.h"

#include "Renderer/RenderTypes.h"

struct GeometryRenderingUnit {
	MeshId meshId;
	MaterialId materialId{INVALID_MATERIAL_ID};

	glm::mat4 transformMatrix;

	Primitives::Sphere worldBoundingSphere;

	bool castShadows{ true };
};