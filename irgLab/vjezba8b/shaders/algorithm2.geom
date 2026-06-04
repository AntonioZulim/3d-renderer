#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 worldPos[];
in vec3 color[];
out vec3 fragColor;

uniform vec3 eye;

void main()
{
	vec3 c = (worldPos[0]+worldPos[1]+worldPos[2])/3; // tocka u centru poligona
	vec3 e = normalize(eye - c); // ociste
	vec3 n = normalize(cross(worldPos[1]-worldPos[0], worldPos[2]-worldPos[0])); // normala poligona

	// uklanjanje straznjih poligona
	if (dot(n, e)>0)
	{
		gl_Position = gl_in[0].gl_Position;
		fragColor = color[0];
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		fragColor = color[1];
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		fragColor = color[2];
		EmitVertex();
		EndPrimitive();
	}
}