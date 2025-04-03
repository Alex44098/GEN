#version 460

#extension GL_GOOGLE_include_directive : require

#include "skyboxConstants.glsl"
#include "bindless.glsl"

layout (location = 0) in vec2 inTexCoords;

layout (location = 0) out vec4 outFragColor;

void main() {
    vec2 ndc = inTexCoords * 2.0 - vec2(1.0);

    vec4 coord = skyboxConstants.invViewProj * vec4(ndc, 1.0, 1.0);
    vec3 samplePoint = normalize(coord.xyz / vec3(coord.w) - skyboxConstants.cameraPos.xyz);

    outFragColor = sampleTextureCubeLinear(skyboxConstants.skyboxTextureId, samplePoint);
}