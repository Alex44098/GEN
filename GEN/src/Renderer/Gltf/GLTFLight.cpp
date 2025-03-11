#include "Renderer/Gltf/GLTFLight.h"

void GLTFLight::SetConeAngles(float innerConeAngle, float outerConeAngle) {
	// From KHR_lights_punctual
	scaleOffset.x = 1.f / std::max(0.001f, std::cos(innerConeAngle) - std::cos(outerConeAngle));
	scaleOffset.y = -std::cos(outerConeAngle) * scaleOffset.x;
}

int GLTFLight::GetTypeCode() const {
    switch (type) {
    case GLTFLightType::Directional:
        return 0;
    case GLTFLightType::Point:
        return 1;
    case GLTFLightType::Spot:
        return 2;
    default:
        assert(false);
    }
    return 0;
}