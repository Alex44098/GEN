#include "Renderer/CameraSystem.h"
#include "Events/MouseMoveEvent.h"
#include "Events/KeyDownEvent.h"

CameraSystem::CameraSystem(Camera& camera) : camera(camera){
	GECS::GECSInstance->GetEventQueue()->AddEventHandler<MouseMoveEvent>(this, &CameraSystem::SetMousePos);
	GECS::GECSInstance->GetEventQueue()->AddEventHandler<KeyDownEvent>(this, &CameraSystem::SetCameraPos);
}

CameraSystem::~CameraSystem() {}

void CameraSystem::Update(GECS::f32 delta) {
	if (this->firstFrame) {
		this->firstFrame = false;
		return;
	}

	GECS::f32 offsetX = this->mouseRelX;
	GECS::f32 offsetY = this->mouseRelY;

	// sensitivity
	offsetX *= 0.4f * delta;
	offsetY *= 0.4f * delta;

	this->yaw += offsetX;
	this->pitch += offsetY;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	this->camera.SetYawPitch(-this->yaw, -this->pitch);

	glm::vec3 moveVector{};
	moveVector += this->camera.GetTransform().GetLocalFront() * moveDirection.y;
	moveVector += this->camera.GetTransform().GetLocalRight() * moveDirection.x;
	this->camera.SetPosition(this->camera.GetPosition() + moveVector * 20.f * delta);

	//this->prevMouseX = this->mouseX;
	//this->prevMouseY = this->mouseY;
	this->mouseRelX = 0.f;
	this->mouseRelY = 0.f;
	this->moveDirection.x = 0.f;
	this->moveDirection.y = 0.f;
}

void CameraSystem::SetMousePos(const GECS::Event::IEvent* e) {
	const MouseMoveEvent* event = reinterpret_cast<const MouseMoveEvent*>(e);

	this->mouseRelX = event->mouseRelX;
	this->mouseRelY = event->mouseRelY;
	//SDL_GetGlobalMouseState(&this->mouseX, &this->mouseY);
	//this->mouseX = event->mouseX;
	//this->mouseY = event->mouseY;
}

void CameraSystem::SetCameraPos(const GECS::Event::IEvent* e) {
	const KeyDownEvent* event = reinterpret_cast<const KeyDownEvent*>(e);

	switch (event->keyCode) {
		case SDLK_w:
			this->moveDirection.y = 1.f;
			break;
		case SDLK_s:
			this->moveDirection.y = -1.f;
			break;
		
		case SDLK_a:
			this->moveDirection.x = -1.f;
			break;
		case SDLK_d:
			this->moveDirection.x = 1.f;
			break;
	}
}