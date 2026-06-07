#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPos[];
in vec3 norm[];
in vec3 tang[];
in vec3 bitang[];
in vec2 uv[];
out vec3 fragNorm;
out vec3 fragTang;
out vec3 fragBitang;
out vec2 fragUv;
out vec3 fragWorldPos;

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
		fragNorm = norm[0];
		fragTang = tang[0];
		fragBitang = bitang[0];
		fragUv = uv[0];
		fragWorldPos = worldPos[0];
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		fragNorm = norm[1];
		fragTang = tang[1];
		fragBitang = bitang[1];
		fragUv = uv[1];
		fragWorldPos = worldPos[1];
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		fragNorm = norm[2];
		fragTang = tang[2];
		fragBitang = bitang[2];
		fragUv = uv[2];
		fragWorldPos = worldPos[2];
		EmitVertex();
		EndPrimitive();
	}
}