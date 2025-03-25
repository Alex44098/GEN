#ifndef GEN_SHADER_SCENE_DATA
#define GEN_SHADER_SCENE_DATA

#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout: require

#include "material.glsl"
#include "light.glsl"

layout (buffer_reference, scalar) readonly buffer LightDataBuffer {
    Light data[];
};

layout (buffer_reference, scalar) readonly buffer SceneDataBuffer {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraPos;

    vec3 ambientColor;
    float ambientIntensity;
    vec3 fogColor;
    float fogIntensity;

    MaterialsBuffer materialsBuffer;

    LightDataBuffer lightsBuffer;
    int numLights;
    int sunlightIndex;
    float pointLightFarPlane;
} sceneDataBuffer;

#endif // GEN_SHADER_SCENE_DATA