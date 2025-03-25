#version 460

#extension GL_GOOGLE_include_directive : require

#include "depthResolveConstants.glsl"
#include "bindless.glsl"

layout (location = 0) in vec2 inTexCoords;

void main() {
    ivec2 pixel = ivec2(inTexCoords * depthResolveConstants.depthImageSize);

    float depth = 1.0;
    for (int i = 0; i < depthResolveConstants.samples; i++) {
        depth = min(depth, sampleTexture2DMSNearest(depthResolveConstants.depthImageId, pixel, i).r);
    }

    gl_FragDepth = depth;
}