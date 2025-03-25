#extension GL_EXT_scalar_block_layout: require

layout (push_constant, scalar) uniform constants {
    SceneDataBuffer sceneData;
    uint drawImage;
    uint depthImage;
} postFXConstants;