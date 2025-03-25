#extension GL_EXT_scalar_block_layout: require

layout (push_constant, scalar) uniform constants {
    vec2 depthImageSize;
    uint depthImageId;
    int samples;
} depthResolveConstants;