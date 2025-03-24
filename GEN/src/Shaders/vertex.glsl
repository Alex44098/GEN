#ifndef GEN_VERTEX
#define GEN_VERTEX

#extension GL_EXT_buffer_reference : require

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 texCoord;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

#endif // GEN_VERTEX