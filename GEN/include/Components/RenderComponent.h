#pragma once

#include <GECSHeaders.h>

#include "Renderer/RenderTypes.h"
#include "Math/Transform.h"

class RenderComponent : public GECS::Component<RenderComponent> {
public:
	std::vector<MeshId> meshes;
	std::vector<MaterialId> materials;
	std::vector<Transform> transforms;
	bool castShadow{ true };

	RenderComponent();
	virtual ~RenderComponent();
};