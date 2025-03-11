#include "Math/Transform.h"

namespace {
    const glm::mat4 unitMatrix = glm::mat4{1.f};
}

Transform::Transform(const glm::mat4& matrix)
{
    glm::vec3 scaleVec;
    glm::quat rotationQuat;
    glm::vec3 translationVec;
    glm::vec3 skewVec;
    glm::vec4 perspectiveVec;
    glm::decompose(matrix, scaleVec, rotationQuat, translationVec, skewVec, perspectiveVec);

    this->position = translationVec;
    this->rotation = rotationQuat;
    scale = scaleVec;

    this->needUpdate = true;
}

Transform Transform::operator*(const Transform& other) const
{
    if (this->GetMatrix() == unitMatrix) {
        return other;
    }
    if (other.GetMatrix() == unitMatrix) {
        return *this;
    }
    return Transform(GetMatrix() * other.GetMatrix());
}

Transform Transform::Inverse() const
{
    if (this->GetMatrix() == unitMatrix) {
        return Transform{};
    }
    return Transform(glm::inverse(this->GetMatrix()));
}

const glm::mat4& Transform::GetMatrix() const
{
    if (!this->needUpdate) {
        return transformMatrix;
    }

    this->transformMatrix = glm::translate(unitMatrix, this->position);
    if (this->rotation != glm::identity<glm::quat>()) {
        this->transformMatrix *= glm::mat4_cast(this->rotation);
    }
    this->transformMatrix = glm::scale(this->transformMatrix, this->scale);
    this->needUpdate = false;
    return this->transformMatrix;
}

void Transform::SetPosition(const glm::vec3& pos)
{
    this->position = pos;
    this->needUpdate = true;
}

void Transform::SetRotation(const glm::quat& rot)
{
    // non-normalized quaternions cause all sorts of issues
    this->rotation = glm::normalize(rot);
    this->needUpdate = true;
}

void Transform::SetScale(const glm::vec3& sc)
{
    this->scale = sc;
    this->needUpdate = true;
}