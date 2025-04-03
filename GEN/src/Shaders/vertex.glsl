#ifndef GEN_VERTEX
#define GEN_VERTEX

#extension GL_EXT_buffer_reference2 : require

struct Vertex {
    vec3 position;
    float texCoordX;
    vec3 normal;
    float texCoordY;
    vec4 tangent;
};
// struct Vertex {
//     vec3 position;
//     vec3 normal;
//     vec4 tangent;
//     vec2 texCoords;
// };

layout (buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

#endif // GEN_VERTEX