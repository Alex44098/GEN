#pragma once

#include <glm.hpp>

#include "Renderer/RenderTypes.h"

struct GeometryRenderingUnit {
	MeshId meshId;
	MaterialId materialId{INVALID_MATERIAL_ID};

	glm::mat4 transformMatrix;

	bool castShadows{ true };
};