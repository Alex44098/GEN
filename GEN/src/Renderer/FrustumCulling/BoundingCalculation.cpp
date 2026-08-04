#include "Renderer/FrustumCulling/BoundingCalculation.h"

#include "Renderer/FrustumCulling/Frustum.h"
#include "Math/GeometricPrimitive/AABB.h"
#include "Math/GeometricPrimitive/Sphere.h"
#include "Math/GeometricPrimitive/Plane.h"

// For length2
#include <gtx/norm.hpp>

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
    // Вычисляет приближённую минимальную охватывающую сферу для набора точек.
    // Используется эвристика Риттера: сначала грубая оценка по 13 направлениям,
    // затем проход по всем точкам с расширением сферы при необходимости.
    [[nodiscard]]
    Primitives::Sphere CalculateBoundingSphere(std::span<const glm::vec3> positions)
    {
        assert(!positions.empty());

        // 13 фиксированных направлений для поиска пары наиболее разнесённых точек.
        // Направления охватывают октанты пространства и основные оси.
        constexpr std::array<glm::vec3, 13> directions = { {
            {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f},
            {1.f, 1.f, 0.f}, {1.f, 0.f, 1.f}, {0.f, 1.f, 1.f},
            {1.f,-1.f, 0.f}, {1.f, 0.f,-1.f}, {0.f, 1.f,-1.f},
            {1.f, 1.f, 1.f}, {1.f,-1.f, 1.f}, {1.f, 1.f,-1.f}, {1.f,-1.f,-1.f}
        } };

        // Индексы вершин с минимальной и максимальной проекцией для каждого направления.
        std::array<std::size_t, directions.size()> minIndex{}, maxIndex{};

        // Для каждого направления находим точки, дающие минимальную и максимальную скалярную проекцию.
        for (std::size_t dirIdx = 0; dirIdx < directions.size(); ++dirIdx) {
            const auto& dir = directions[dirIdx];

            std::size_t minI = 0, maxI = 0;
            float minProj = glm::dot(dir, positions[0]);
            float maxProj = minProj;

            for (std::size_t i = 1; i < positions.size(); ++i) {
                const float proj = glm::dot(dir, positions[i]);
                if (proj < minProj) {
                    minProj = proj;
                    minI = i;
                }
                else if (proj > maxProj) {
                    maxProj = proj;
                    maxI = i;
                }
            }
            minIndex[dirIdx] = minI;
            maxIndex[dirIdx] = maxI;
        }

        // Выбираем направление, вдоль которого расстояние между экстремальными точками максимально.
        std::size_t bestDir = 0;
        float maxDistSq = 0.f;
        for (std::size_t dirIdx = 0; dirIdx < directions.size(); ++dirIdx) {
            const float distSq = glm::length2(positions[maxIndex[dirIdx]] - positions[minIndex[dirIdx]]);
            if (distSq > maxDistSq) {
                maxDistSq = distSq;
                bestDir = dirIdx;
            }
        }

        const auto& A = positions[minIndex[bestDir]];
        const auto& B = positions[maxIndex[bestDir]];
        Primitives::Sphere sphere{
            .center = (A + B) * 0.5f,
            .radius = std::sqrt(maxDistSq) * 0.5f
        };

        // Расширяем сферу, если какие-то точки оказались вне неё.
        for (const auto& point : positions) {
            const glm::vec3 diff = point - sphere.center;
            const float distSq = glm::length2(diff);
            if (distSq > sphere.radius * sphere.radius) {
                // Перемещаем центр в сторону новой точки и увеличиваем радиус.
                const glm::vec3 directionToCenter = glm::normalize(-diff); // от точки к текущему центру
                const glm::vec3 newCenter = point + directionToCenter * sphere.radius;
                sphere.center = (newCenter + point) * 0.5f;
                sphere.radius = glm::length(newCenter - sphere.center);
            }
        }

        return sphere;
    }

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