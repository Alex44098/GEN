#version 460

#extension GL_GOOGLE_include_directive : require

#include "bindless.glsl"
#include "shaderSceneData.glsl"
#include "meshConstants.glsl"
#include "material.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoords;
//layout (location = 2) in vec3 inNormal;
//layout (location = 3) in vec4 inTangent;
//layout (location = 4) in mat3 inTBN;

layout (location = 0) out vec4 outFragColor;

void main() {
    MaterialData material = meshConstants.sceneData.materialsBuffer.data[meshConstants.materialId];
    
    vec4 diffuse = sampleTexture2DLinear(material.diffuseTexture, inTexCoords);
    //vec4 diffuse = sampleTexture2DLinear(2, inTexCoords);

    outFragColor = diffuse;

    //outFragColor = vec4(0.0, 1.0, 0.0, 1.0);
}