#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in vec3 worldPos[];

uniform vec3 eye;

void main()
{
	vec3 c = (worldPos[0]+worldPos[1]+worldPos[2])/3;
	vec3 e = eye - c;
	vec3 n = cross(worldPos[1]-worldPos[0], worldPos[2]-worldPos[0]);
	if (dot(n, e)>0)
	{
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		EndPrimitive();
	}
}