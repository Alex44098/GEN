#ifndef GEN_LIGHT
#define GEN_LIGHT

#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2

struct Light {
    vec3 position;
    uint type;
    vec3 direction;
    float range;
    vec3 color;
    float intensity;
    vec2 scaleOffset;
    float unused;
};

#endif // GEN_SHADER_SCENE_DATA