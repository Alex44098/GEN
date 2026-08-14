#pragma once

#include "glm.hpp"

#include "Math/GeometricPrimitive/Sphere.h"

#include "Renderer/Vulkan/Buffer.h"
#include "Renderer/RenderTypes.h"

struct Mesh {
	struct Vertex {
		glm::vec3 position;
		float texCoordX{};
		glm::vec3 normal;
		float texCoordY{};
		glm::vec4 tangent;
	};

	std::vector<Vertex> vertices;
	std::vector<GECS::u32> indices;
	std::string name;

	Primitives::Sphere boundingSphere;

	// buffers on GPU
	Buffer vertexBuffer;
	Buffer indexBuffer;
};