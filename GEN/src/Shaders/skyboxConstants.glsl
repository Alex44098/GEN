layout (push_constant) uniform constants {
    mat4 invViewProj;
    vec3 cameraPos;
    uint skyboxTextureId;
} skyboxConstants;