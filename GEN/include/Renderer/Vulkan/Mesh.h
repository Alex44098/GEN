#pragma once

#include "glm.hpp"

#include "Renderer/Vulkan/Buffer.h"
#include "Engine/EngineTypes.h"

struct Mesh {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec2 texCoord;
	};

	std::vector<Vertex> vertices;
	std::vector<GECS::u32> indices;
	std::string name;

	// buffers on GPU
	Buffer vertexBuffer;
	Buffer indexBuffer;
};