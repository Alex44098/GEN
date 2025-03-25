#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout: require

#include "shaderSceneData.glsl"
#include "vertex.glsl"

layout (push_constant, scalar) uniform constants {
    mat4 transform;
    SceneDataBuffer sceneData;
    VertexBuffer vertexBuffer;
    uint materialId;
} meshConstants;