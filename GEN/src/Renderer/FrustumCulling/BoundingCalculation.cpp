#include "Renderer/FrustumCulling/BoundingCalculation.h"

#include "Renderer/FrustumCulling/Frustum.h"
#include "Math/GeometricPrimitive/AABB.h"
#include "Math/GeometricPrimitive/Sphere.h"
#include "Math/GeometricPrimitive/Plane.h"

namespace {
    // The sphere is not completely behind the plane if the sign distance to the center is > -radius
    bool IsOnOrForwardPlane(const Primitives::Plane& plane, const Primitives::Sphere& sphere) {
        return plane.getSignedDistanceToPlane(sphere.center) > -sphere.radius;
    }

    glm::vec3 GetTransformScale(const glm::mat4& transform) {
        return {
            glm::length(glm::vec3{transform[0][0], transform[0][1], transform[0][2]}),
            glm::length(glm::vec3{transform[1][0], transform[1][1], transform[1][2]}),
            glm::length(glm::vec3{transform[2][0], transform[2][1], transform[2][2]})
        };
    }
}

namespace FrustumCulling {
    bool IsInFrustum(const Frustum& frustum, const Primitives::Sphere& s) {
        for (const Primitives::Plane& plane : frustum.planes) {
            if (!IsOnOrForwardPlane(plane, s)) {
                return false;
            }
        }
        return true;
    }

    bool IsInFrustum(const Frustum& frustum, const Primitives::AABB& aabb) {
        for (const Primitives::Plane& plane : frustum.planes) {
            // p-vertex: the vertex AABB that is as far away as possible in the direction of the plane normal.
            // If it is behind the plane(signedDistance < 0), the entire AABB is outside.
            const glm::vec3 pVertex{
                plane.normal.x > 0 ? aabb.max.x : aabb.min.x,
                plane.normal.y > 0 ? aabb.max.y : aabb.min.y,
                plane.normal.z > 0 ? aabb.max.z : aabb.min.z,
            };

            if (plane.getSignedDistanceToPlane(pVertex) < 0) {
                return false;
            }
        }
        return true;
    }

    Primitives::Sphere CalculateBoundingSphereWorld(
        const glm::mat4& transform,
        const Primitives::Sphere& s) {

        const glm::vec3 scale = GetTransformScale(transform);
        float maxScale = std::max({ scale.x, scale.y, scale.z });

        Primitives::Sphere sphereWorld = s;
        sphereWorld.radius *= maxScale;
        sphereWorld.center = glm::vec3(transform * glm::vec4(sphereWorld.center, 1.f));
        return sphereWorld;
    }
}