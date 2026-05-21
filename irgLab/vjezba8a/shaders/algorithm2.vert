#version 330 core
layout (location = 0) in vec3 vPos;

out vec3 worldPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 perspectiveMat;

void main()
{
    worldPos = vec3(modelMat * vec4(vPos, 1.0));
    gl_Position = perspectiveMat * viewMat * vec4(worldPos, 1.0);
}