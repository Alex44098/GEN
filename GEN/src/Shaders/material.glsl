#ifndef GEN_MATERIAL
#define GEN_MATERIAL

#extension GL_EXT_buffer_reference2 : require

struct MaterialData {
    vec4 baseColor;
    vec4 factors;
    uint diffuseTexture;
    uint normalTexture;
    uint metallicRoughTexture;
    uint emissiveTexture;
};

layout (buffer_reference, std430) readonly buffer MaterialsBuffer {
    MaterialData data[];
} materialsBuffer;

#endif // GEN_MATERIAL