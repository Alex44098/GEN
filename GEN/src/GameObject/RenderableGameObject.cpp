#include "GameObject/RenderableGameObject.h"

RenderableGameObject::RenderableGameObject() {
	this->AddComponent<RenderComponent>();
}

RenderableGameObject::~RenderableGameObject() {
	this->RemoveComponent<RenderComponent>();
}