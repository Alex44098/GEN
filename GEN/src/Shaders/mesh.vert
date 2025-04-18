#version 460

#extension GL_GOOGLE_include_directive : require

#include "meshConstants.glsl"

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec2 outTexCoords;
//layout (location = 2) out vec3 outNormal;
//layout (location = 3) out vec4 outTangent;
//layout (location = 4) out mat3 outTBN;

void main()
{
    Vertex vertex = meshConstants.vertexBuffer.vertices[gl_VertexIndex];

    vec4 worldPos = meshConstants.transform * vec4(vertex.position, 1.0f);

    gl_Position = meshConstants.sceneData.viewProjection * worldPos;
    //gl_Position.z = 1.0 / gl_Position.z;
    outPosition = worldPos.xyz;

    //outTexCoords = vertex.texCoords;
    outTexCoords = vec2(vertex.texCoordX, vertex.texCoordY);
    //outNormal = mat3(transpose(inverse(meshConstants.transform))) * vertex.normal;

    //outTangent = vertex.tangent;

    //vec3 T = normalize(vec3(meshConstants.transform * vertex.tangent));
    //vec3 N = normalize(outNormal);
    //vec3 B = cross(N, T) * vertex.tangent.w;
    //outTBN = mat3(T, B, N);
}