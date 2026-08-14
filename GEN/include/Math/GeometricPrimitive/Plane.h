#pragma once

#include <glm.hpp>

namespace Primitives
{
    /// <summary>
    /// Plane primitive.
    /// </summary>
    struct Plane {
        glm::vec3 normal{ 0.f, 1.f, 0.f };
        float distance{ 0.f };

        Plane() = default;
        
        // p1 – point on plane
        // norm – normal direction
        Plane(const glm::vec3& p1, const glm::vec3& norm)
            : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

        // Signed distance from point to plane: > 0 – point onm front (frustum inside)
        [[nodiscard]]
        float getSignedDistanceToPlane(const glm::vec3& point) const {
            return glm::dot(normal, point) - distance;
        }
    };
}