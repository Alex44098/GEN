#pragma once

// For lights from GLTF 2.0 used KHR_lights_punctural extension
// Read more: https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md

#include "Renderer/RenderTypes.h"
#include "glm.hpp"

namespace Gltf {
	enum class GLTFLightType {
		None,
		Directional,
		Point,
		Spot
	};

	struct GLTFLightData {
		glm::vec3 position;
		std::uint32_t type;
		glm::vec3 direction;
		float range;
		LinearColorWithoutAlpha color;
		float intensity;
		glm::vec2 scaleOffset;
		float unused;
	};

	struct GLTFLight {
		std::string name;
		GLTFLightType type{ GLTFLightType::None };
		LinearColor color;
		float range{ 0.f };
		float intensity{ 0.f };
		glm::vec2 scaleOffset;

		void SetConeAngles(float innerConeAngle, float outerConeAngle);

		int GetTypeCode() const;
	};
}
