#version 330 core
layout (location = 0) in vec3 vPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 perspectiveMat;

void main()
{
    gl_Position = perspectiveMat * viewMat * modelMat * vec4(vPos, 1.0);
}