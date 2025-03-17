#pragma once

// See more: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#scenes

#include <filesystem>

#include "Renderer/Gltf/GLTFLight.h"
#include "Renderer/Vulkan/Mesh.h"
#include "Renderer/RenderTypes.h"
#include "Math/Transform.h"
#include "Renderer/Camera.h"

namespace Gltf {
	struct GLTFSceneNode {
		std::string name;
		Transform transform;
		GECS::i32 meshIndex{ -1 };
		GECS::i32 lightIndex{ -1 };

		GLTFSceneNode* parent{ nullptr };
		std::vector<std::unique_ptr<GLTFSceneNode>> children;
	};

	struct GLTFSceneMesh {
		std::vector<MeshId> primitives;
		std::vector<MaterialId> primitiveMaterials;
	};

	struct GLTFScene {
		std::filesystem::path path;

		// root nodes
		std::vector<std::unique_ptr<GLTFSceneNode>> nodes;

		std::vector<GLTFSceneMesh> sceneMeshes;
		std::vector<GLTFLight> lights;
	};

	struct GLTFSceneData {
		const Camera& camera;
		LinearColor ambientColor;
		GECS::f32 ambientIntensity;
		LinearColor fogColor;
		GECS::f32 fogIntensity;
	};

	struct GLTFShaderSceneData {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
		glm::vec4 cameraPos;

		LinearColorWithoutAlpha ambientColor;
		GECS::f32 ambientIntensity;
		LinearColorWithoutAlpha fogColor;
		GECS::f32 fogIntensity;

		VkDeviceAddress materialsBuffer; // for bindless textures

		// Lightings
		VkDeviceAddress lightsBuffer;
		GECS::u32 numLights;
		GECS::u32 sunIndex;
		GECS::f32 pointLightFarPlane;
	};
}
