#pragma once

#include <glm.hpp>

#include "Math/Transform.h"

class Camera {
	Transform transform;
	glm::mat4 projection;

	bool isOrtho3d{ false };
	bool isOrtho2d{ false };

	bool useInverseDepth{ false };
	bool clipSpaceYDown{ true };

	GECS::f32 nearZ{ 1.f };
	GECS::f32 farZ{ 75.f };
	GECS::f32 aspectRatio{ 16.f / 9.f }; // classic
	GECS::f32 fovX{ glm::radians(90.f) };
	GECS::f32 fovY{ glm::radians(60.f) };

	GECS::f32 orthoScaleX;
	GECS::f32 orthoScaleY;
	glm::vec2 orthoViewSize;

public:
	void Init(GECS::f32 fovX, GECS::f32 nearZ, GECS::f32 farZ, GECS::f32 aspectRatio);
	void InitOrtho3d(GECS::f32 scaleX, GECS::f32 scaleY, GECS::f32 nearZ, GECS::f32 farZ);
	void InitOrtho2d(const glm::vec2& size, GECS::f32 nearZ, GECS::f32 farZ, bool isCenter);

	glm::mat4 GetView() const;
	glm::mat4 GetViewProjection() const;

	inline const Transform& GetTransform() const { return this->transform; }

	void SetPosition(const glm::vec3& pos) { return this->transform.SetPosition(pos); }
	inline const glm::vec3 GetPosition() const { return this->transform.GetPosition(); }

	void SetPosition2D(const glm::vec2& pos) { this->transform.SetPosition({ pos, 0.f }); }
	inline const glm::vec2 GetPosition2D() const { return glm::vec2{ this->transform.GetPosition() }; }

	void SetRotation(const glm::quat& rot) { this->transform.SetRotation(rot); }
	inline const glm::quat& GetRotation() const { return this->transform.GetRotation(); }

	void SetProjection(const glm::mat4& proj) { this->projection = proj; }
	inline const glm::mat4& GetProjection() const { return this->projection; }

	inline bool IsOrthographic3D() const { return this->isOrtho3d; }
	inline bool IsOrthographic2D() const { return this->isOrtho2d; }

	inline GECS::f32 GetZFar() const { return this->farZ; };
	inline GECS::f32 GetZNear() const { return this->nearZ; };
	inline GECS::f32 GetAspectRatio() const { return this->aspectRatio; }
	inline GECS::f32 GetFOVX() const { return this->fovX; }
	inline GECS::f32 GetFOVY() const { return this->fovY; }

	void SetYawPitch(GECS::f32 yaw, GECS::f32 pitch);

	void SetUseInverseDepth(bool b);
	inline bool UseInverseDepth() const { return this->useInverseDepth; }

	void SetClipSpaceYDown(bool b);
	inline bool IsClipSpaceYDown() const { return this->clipSpaceYDown; }
};