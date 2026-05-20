#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

void main()
{
	vec2 v0 = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
	vec2 v1 = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
	vec2 v2 = gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

	vec2 b1 = v1 - v0;
	vec2 b2 = v2 - v0;
	float det = b1.x*b2.y - b2.x*b1.y;
	if (det>0)
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