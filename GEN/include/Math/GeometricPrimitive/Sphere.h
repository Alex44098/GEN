#pragma once

#include <glm.hpp>

namespace Primitives {

    /// <summary>
    /// Sphere primitive.
    /// </summary>
    struct Sphere {
        glm::vec3 center{};
        float radius{};
    };
}