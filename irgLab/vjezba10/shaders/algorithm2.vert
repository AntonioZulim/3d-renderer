#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTang;
layout (location = 4) in vec3 vBitang;

#define MAX_LIGHTS 10

out VS_OUT {
    vec3 norm;
    vec3 tang;
    vec3 bitang;
    vec2 uv;
    vec3 worldPos;
    vec4 lightPos[MAX_LIGHTS];
} vs_out;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 perspectiveMat;
uniform mat4 normalMat;

uniform mat4 lightViewMat[MAX_LIGHTS];
uniform mat4 lightPerspectiveMat[MAX_LIGHTS];
uniform uint numLights;

void main()
{
    vs_out.norm = normalize(vec3(normalMat * vec4(vNorm, 0)));
    vs_out.tang = normalize(vec3(normalMat * vec4(vTang, 0.0)));
    vs_out.bitang = normalize(vec3(normalMat * vec4(vBitang, 0.0)));
    vs_out.uv = vUv;
    vec3 worldPos = vec3(modelMat * vec4(vPos, 1.0));
    vs_out.worldPos = worldPos;
    for (uint i = 0u; i<numLights; i++){
        vs_out.lightPos[i] = lightPerspectiveMat[i] * lightViewMat[i] * vec4(worldPos, 1.0);
    }
    gl_Position = perspectiveMat * viewMat * vec4(worldPos, 1.0);
}