#include "Renderer/FrustumCulling/Frustum.h"

#include "Math/GeometricPrimitive/AABB.h"
#include "Math/GeometricPrimitive/Sphere.h"
#include "Math/GeometricPrimitive/Plane.h"
#include "Renderer/Camera.h"

#include <algorithm>
#include <array>

namespace {
    // Finds the center of the face by the four indices of the angles of the frustrum
    glm::vec3 FindCenter(const std::array<glm::vec3, 8>& points,
        const std::array<int, 4>& indices) {
        return (points[indices[0]] + points[indices[1]] +
            points[indices[2]] + points[indices[3]]) / 4.f;
    }

    // The normal of a face, constructed along two edges.
    // The direction depends on the traversal order.
    glm::vec3 FindNormal(const std::array<glm::vec3, 8>& points,
        const std::array<int, 4>& indices) {
        const auto e1 = glm::normalize(points[indices[1]] - points[indices[0]]);
        const auto e2 = glm::normalize(points[indices[2]] - points[indices[1]]);
        return glm::cross(e1, e2);
    }
}

namespace FrustumCulling {
    std::array<glm::vec3, 8> CalculateFrustumCornersWorldSpace(const Camera& camera) {
        // Depth in NDC (Normalized Device Coordinates) with camera inversion
        const float nearDepth = camera.UseInverseDepth() ? 1.0f : 0.f;
        const float farDepth = camera.UseInverseDepth() ? 0.0f : 1.f;

        // Y direction in NDC
        const float bottomY = camera.IsClipSpaceYDown() ? 1.f : -1.f;
        const float topY = camera.IsClipSpaceYDown() ? -1.f : 1.f;

        const std::array<glm::vec3, 8> cornersNDC = { {
                // near plane
                {-1.f, bottomY, nearDepth},
                {-1.f, topY,    nearDepth},
                { 1.f, topY,    nearDepth},
                { 1.f, bottomY, nearDepth},
                // far plane
                {-1.f, bottomY, farDepth},
                {-1.f, topY,    farDepth},
                { 1.f, topY,    farDepth},
                { 1.f, bottomY, farDepth},
            } };

        const auto invViewProj = glm::inverse(camera.GetViewProjection());
        std::array<glm::vec3, 8> cornersWorld;
        for (int i = 0; i < 8; ++i) {
            auto corner = invViewProj * glm::vec4(cornersNDC[i], 1.f);
            corner /= corner.w;
            cornersWorld[i] = glm::vec3(corner);
        }
        return cornersWorld;
    }

    Frustum CreateFrustumFromCamera(const Camera& camera) {
        Frustum frustum{};
        // Indicies for array filling
        enum FrustumPlaneIdx : int { Far = 0, Near, Left, Right, Top, Bottom };

        if (camera.IsOrthographic3D()) {
            const auto points = CalculateFrustumCornersWorldSpace(camera);

            // Индексы углов для каждой грани (обход против часовой для лицевой стороны)
            constexpr std::array<int, 4> nearFace = { 0, 1, 2, 3 };
            constexpr std::array<int, 4> farFace = { 7, 6, 5, 4 };
            constexpr std::array<int, 4> leftFace = { 4, 5, 1, 0 };
            constexpr std::array<int, 4> rightFace = { 3, 2, 6, 7 };
            constexpr std::array<int, 4> bottomFace = { 4, 0, 3, 7 };
            constexpr std::array<int, 4> topFace = { 5, 6, 2, 1 };

            frustum.planes[Far] = {
                FindCenter(points, farFace),
                FindNormal(points, farFace)
            };
            frustum.planes[Near] = {
                FindCenter(points, nearFace),
                FindNormal(points, nearFace)
            };
            frustum.planes[Left] = {
                FindCenter(points, leftFace),
                FindNormal(points, leftFace)
            };
            frustum.planes[Right] = {
                FindCenter(points, rightFace),
                FindNormal(points, rightFace)
            };
            frustum.planes[Top] = {
                FindCenter(points, topFace),
                FindNormal(points, topFace)
            };
            frustum.planes[Bottom] = {
                FindCenter(points, bottomFace),
                FindNormal(points, bottomFace)
            };
        }
        else {
            // Перспективная камера: строим плоскости по точке камеры и направлениям
            const auto camPos = camera.GetPosition();
            const auto camFront = camera.GetTransform().GetLocalFront();
            const auto camUp = camera.GetTransform().GetLocalUp();
            const auto camRight = camera.GetTransform().GetLocalRight();

            const float zNear = camera.GetZNear();
            const float zFar = camera.GetZFar();
            const float halfVSide = zFar * tanf(camera.GetFOVY() * 0.5f);
            const float halfHSide = halfVSide * camera.GetAspectRatio();
            const auto  frontMultFar = zFar * camFront;

            // Ближняя и дальняя плоскости
            frustum.planes[Near] = { camPos + zNear * camFront, camFront };
            frustum.planes[Far] = { camPos + frontMultFar, -camFront };

            // Боковые плоскости. Нормали направлены внутрь фрустума.
            frustum.planes[Left] = { camPos, glm::cross(camUp, frontMultFar + camRight * halfHSide) };
            frustum.planes[Right] = { camPos, glm::cross(frontMultFar - camRight * halfHSide, camUp) };
            frustum.planes[Bottom] = { camPos, glm::cross(frontMultFar + camUp * halfVSide, camRight) };
            frustum.planes[Top] = { camPos, glm::cross(camRight, frontMultFar - camUp * halfVSide) };
        }

        return frustum;
    }
}