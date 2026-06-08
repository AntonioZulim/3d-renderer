#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

#define MAX_LIGHTS 10

in VS_OUT {
	vec3 norm;
	vec3 tang;
	vec3 bitang;
	vec2 uv;
	vec3 worldPos;
	vec4 lightPos[MAX_LIGHTS];
} gs_in[];

out vec3 fragNorm;
out vec3 fragTang;
out vec3 fragBitang;
out vec2 fragUv;
out vec3 fragWorldPos;
out vec4 fragLightPos[MAX_LIGHTS];

uniform vec3 eye;

void main()
{
	vec3 c = (gs_in[0].worldPos+gs_in[1].worldPos+gs_in[2].worldPos)/3; // tocka u centru poligona
	vec3 e = normalize(eye - c); // ociste
	vec3 n = normalize(cross(gs_in[1].worldPos-gs_in[0].worldPos, gs_in[2].worldPos-gs_in[0].worldPos)); // normala poligona

	// uklanjanje straznjih poligona
	if (dot(n, e)>0)
	{
		gl_Position = gl_in[0].gl_Position;
		fragNorm = gs_in[0].norm;
		fragTang = gs_in[0].tang;
		fragBitang = gs_in[0].bitang;
		fragUv = gs_in[0].uv;
		fragWorldPos = gs_in[0].worldPos;
		fragLightPos = gs_in[0].lightPos;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		fragNorm = gs_in[1].norm;
		fragTang = gs_in[1].tang;
		fragBitang = gs_in[1].bitang;
		fragUv = gs_in[1].uv;
		fragWorldPos = gs_in[1].worldPos;
		fragLightPos = gs_in[1].lightPos;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		fragNorm = gs_in[2].norm;
		fragTang = gs_in[2].tang;
		fragBitang = gs_in[2].bitang;
		fragUv = gs_in[2].uv;
		fragWorldPos = gs_in[2].worldPos;
		fragLightPos = gs_in[2].lightPos;
		EmitVertex();
		EndPrimitive();
	}
}