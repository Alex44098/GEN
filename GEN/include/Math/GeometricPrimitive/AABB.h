#pragma once

#include <glm.hpp>

namespace Primitives {

    /// <summary>
    /// Axis-aligned bounding box (for simple bounding calculation).
    /// </summary>
    struct AABB {
        glm::vec3 min{};
        glm::vec3 max{};

        [[nodiscard]]
        glm::vec3 calculateSize() const { return glm::abs(max - min); }
    };
}