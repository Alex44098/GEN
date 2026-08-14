#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout: require

struct DebugVertex {
    vec3 position;
    vec3 color;
};

layout (buffer_reference, scalar) readonly buffer DebugVertexBuffer {
    DebugVertex vertices[];
};

// viewProj + buffer address
layout (push_constant, scalar) uniform constants {
    mat4 viewProj;
    DebugVertexBuffer vertexBuffer;
} pc;

layout (location = 0) out vec3 fragColor;

void main() {
    DebugVertex v = pc.vertexBuffer.vertices[gl_VertexIndex];
    gl_Position = pc.viewProj * vec4(v.position, 1.0);
    fragColor = v.color;
}