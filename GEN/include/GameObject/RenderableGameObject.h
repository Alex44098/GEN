#pragma once

#include "Engine/EngineTypes.h"

#include "Components/RenderComponent.h"

class RenderableGameObject : public GECS::Entity<RenderableGameObject> {
public:
	RenderableGameObject();
	virtual ~RenderableGameObject();
};