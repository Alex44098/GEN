#include "Renderer/Camera.h"
#include "Math/CoordinateSystem.h"

void Camera::Init(GECS::f32 fovX, GECS::f32 nearZ, GECS::f32 farZ, GECS::f32 aspectRatio) {
	// calc fovY
	GECS::f32 g = aspectRatio / glm::tan(fovX / 2.f);
	this->fovY = 2.f * glm::atan(1.f / g);

	this->fovX = fovX;
	this->farZ = farZ;
	this->nearZ = nearZ;
	this->aspectRatio = aspectRatio;

	if (this->useInverseDepth) {
		this->projection = glm::perspective(this->fovY, aspectRatio, farZ, nearZ);
	}
	else {
		this->projection = glm::perspective(this->fovY, aspectRatio, nearZ, farZ);
	}

	if (this->clipSpaceYDown) {
		this->projection[1][1] *= -1;
	}
}

void Camera::InitOrtho3d(GECS::f32 scaleX, GECS::f32 scaleY, GECS::f32 nearZ, GECS::f32 farZ) {
	this->isOrtho3d = true;
	this->farZ = farZ;
	this->nearZ = nearZ;
	this->orthoScaleX = scaleX;
	this->orthoScaleY = scaleY;
	aspectRatio = scaleX / scaleY;

	if (this->useInverseDepth) {
		this->projection = glm::ortho(-scaleX, scaleX, -scaleY, scaleY, farZ, nearZ);
	}
	else {
		this->projection = glm::ortho(-scaleX, scaleX, -scaleY, scaleY, nearZ, farZ);
	}

	if (this->clipSpaceYDown) {
		this->projection[1][1] *= -1;
	}
}

void Camera::InitOrtho2d(const glm::vec2& size, GECS::f32 nearZ, GECS::f32 farZ, bool isCenter) {
	this->SetClipSpaceYDown(true);
	this->isOrtho2d = true;
	this->farZ = farZ;
	this->nearZ = nearZ;
	this->aspectRatio = size.x / size.y;

	if (isCenter)
		this->projection = glm::ortho(-size.x / 2.f, size.x / 2.f, size.y / 2.f, -size.y / 2.f, nearZ, farZ);
	else
		this->projection = glm::ortho(0.f, size.x, 0.f, size.y, nearZ, farZ);

	this->orthoViewSize = size;
}

glm::mat4 Camera::GetView() const {
	if (this->isOrtho2d)
		return glm::translate(glm::mat4{ 1.f }, -this->transform.GetPosition());

	const glm::vec3 up = this->transform.GetLocalUp();
	const glm::vec3 target = this->GetPosition() + this->transform.GetLocalFront();
	return glm::lookAt(this->GetPosition(), target, up);
}

glm::mat4 Camera::GetViewProjection() const {
	return this->projection * this->GetView();
}

void Camera::SetYawPitch(GECS::f32 yaw, GECS::f32 pitch) {
	this->SetRotation(glm::angleAxis(yaw, CoordSystem::upAxis) * glm::angleAxis(pitch, CoordSystem::rightAxis));
}

void Camera::SetUseInverseDepth(bool b) {
	this->useInverseDepth = b;
	if (this->isOrtho3d)
		this->InitOrtho3d(this->orthoScaleX, this->orthoScaleY, this->nearZ, this->farZ);
	else
		this->Init(this->fovX, this->nearZ, this->farZ, this->aspectRatio);
}

void Camera::SetClipSpaceYDown(bool b) {
	this->clipSpaceYDown = b;
	if (this->isOrtho3d)
		this->InitOrtho3d(this->orthoScaleX, this->orthoScaleY, this->nearZ, this->farZ);
	else
		this->Init(this->fovX, this->nearZ, this->farZ, this->aspectRatio);
}