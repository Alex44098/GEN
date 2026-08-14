#pragma once

#include <Volk/volk.h>
#include <glm.hpp>

#include "Renderer/RenderTypes.h"
#include "Renderer/Vulkan/Vulkan.h"

#include "Renderer/FrustumCulling/Frustum.h"
#include "Math/GeometricPrimitive/AABB.h"
#include "Math/GeometricPrimitive/Sphere.h"

#include "Renderer/Vulkan/Pipelines/Pipeline.h"

#include "Renderer/Camera.h"
#include "Renderer/Vulkan/Util/ShaderModuleLoader.h"

class DebugDrawPipeline : public Pipeline {
	struct Constants {
		glm::mat4 viewProj;
		VkDeviceAddress vertexBufferAddress; // buffer address with vertex
	};

	struct DebugDrawVertex {
		glm::vec3 position;
		glm::vec3 color;
	};

	Buffer debugVertexBuffer;
	VkDeviceSize currentBufferSize = 0;

	std::vector<DebugDrawVertex> lineVertices;

public:
	virtual void Init(gvk::Vulkan& vulkan, VkFormat drawImageFormat, VkFormat depthImageFormat, VkSampleCountFlagBits samples) override;
	virtual void Cleanup(gvk::Vulkan& vulkan) override;

	void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
	void AddFrustum(const FrustumCulling::Frustum& frustum, const glm::vec3& color);
	void AddSphere(const Primitives::Sphere& sphere, const glm::vec3& color, int segments = 32);
	void AddAABB(const Primitives::AABB& aabb, const glm::vec3& color);

	void Prepare(gvk::Vulkan& vulkan);

	void Draw(VkCommandBuffer cmdBuffer, gvk::Vulkan& vulkan, const Camera& camera);
	void Clear();
};