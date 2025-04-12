#pragma once

#include "GECSHeaders.h"

#include "Renderer/Camera.h"

class CameraSystem : public GECS::System <CameraSystem> {

	Camera& camera;
	GECS::i32 mouseRelX{ 0 }, mouseRelY{ 0 };
	GECS::f32 yaw{ 0.f }, pitch{ 0.f };

	glm::vec3 moveDirection{};

	bool firstFrame{ false };

public:
	CameraSystem(Camera& camera);
	~CameraSystem();

	virtual void Update(GECS::f32 delta) override;

private:
	void SetMousePos(const GECS::Event::IEvent* e);
	void SetCameraPos(const GECS::Event::IEvent* e);
};