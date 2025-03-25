layout (push_constant) uniform constants {
    mat4 invViewProj;
    vec4 cameraPos;
    uint skyboxTexId;
} skyboxConstants;