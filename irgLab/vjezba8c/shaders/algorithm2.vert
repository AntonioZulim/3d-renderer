#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

out vec3 norm;
out vec3 worldPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 perspectiveMat;
uniform mat4 normalMat;

uniform vec3 eye;

void main()
{
    norm = normalize(vec3(normalMat * vec4(vNorm, 0)));

    worldPos = vec3(modelMat * vec4(vPos, 1.0));
    gl_Position = perspectiveMat * viewMat * vec4(worldPos, 1.0);
}