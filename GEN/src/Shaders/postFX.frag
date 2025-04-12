#version 460

#extension GL_GOOGLE_include_directive : require

layout (location = 0) in vec2 inTexCoords;

#include "bindless.glsl"
#include "shaderSceneData.glsl"
#include "postFXConstants.glsl"

layout (location = 0) out vec4 outFragColor;

vec3 GetViewPos(float depth, mat4 invProj, vec2 texCoords) {
    vec4 clip = invProj * vec4(texCoords * 2.0 - 1.0, depth, 1.0);
    vec3 pos = clip.xyz / clip.w;
    return pos;
}

vec3 ExponentialFog(vec3 pos, vec3 color, vec3 fogColor, float fogIntensity,
                    vec3 ambientColor, float ambientIntensity, vec3 dirLightColor) {
    vec3 fc = fogColor * (ambientColor * ambientIntensity + dirLightColor);
    float dist = length(pos);
    float fogFactor = 1.0 / exp((dist * fogIntensity) * (dist * fogIntensity));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    return mix(fc, color, fogFactor);
}

void main() {
    vec3 fragColor = sampleTexture2DNearest(postFXConstants.drawImage, inTexCoords).rgb;
    //float depth = sampleTexture2DNearest(postFXConstants.depthImage, inTexCoords).r;

    //vec3 sunlightColor = vec3(0, 0, 0);
    //if (postFXConstants.sceneData.sunlightIndex != -1) {
    //    sunlightColor = postFXConstants.sceneData.lightsBuffer.data[postFXConstants.sceneData.sunlightIndex].color;
    //}

    //vec3 viewPos = GetViewPos(depth, inverse(postFXConstants.sceneData.projection), inTexCoords);
    //vec3 color = ExponentialFog(viewPos, fragColor,
    //        postFXConstants.sceneData.fogColor, postFXConstants.sceneData.fogIntensity,
    //        postFXConstants.sceneData.ambientColor, postFXConstants.sceneData.ambientIntensity,
    //        sunlightColor);
    //outFragColor = vec4(color, 1.0);

    outFragColor = vec4(fragColor, 1.0);
}