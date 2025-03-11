#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>

#include "CoordinateSystem.h"

class Transform {
	glm::vec3 position{};
	glm::quat rotation = glm::identity<glm::quat>();
	glm::vec3 scale{1.f};

	mutable glm::mat4 transformMatrix{1.f};
	mutable bool needUpdate{ false };

public:
	Transform() {}
	Transform(const glm::mat4& matrix);

	bool operator==(const Transform& other) const {
		return this->position == other.position &&
			this->rotation == other.rotation && this->scale == other.scale;
	}
	Transform operator*(const Transform& other) const;

	inline glm::vec3 GetLocalUp() const { return this->rotation * CoordSystem::upAxis; }
	inline glm::vec3 GetLocalFront() const { return this->rotation * CoordSystem::frontAxis; }
	inline glm::vec3 GetLocalRight() const { return this->rotation * CoordSystem::rightAxis; }

	const glm::vec3& GetPosition() const { return this->position; }
	const glm::quat& GetRotation() const { return this->rotation; }
	const glm::vec3& GetScale() const { return this->scale; }

	const glm::mat4& GetMatrix() const;

	void SetPosition(const glm::vec3& pos);
	void SetRotation(const glm::quat& rot);
	void SetScale(const glm::vec3& sc);

	Transform Inverse() const;
};