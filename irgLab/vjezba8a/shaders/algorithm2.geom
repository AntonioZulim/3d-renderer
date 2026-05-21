#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 worldPos[];
out vec3 fragColor;

struct Light {
    vec3 position;
    vec3 intensity;
    vec3 ambientIntensity;
};

struct Material {
	vec3 ka, kd, ks;
    float ksn;
};

uniform vec3 eye;
uniform Light light;
uniform Material material;

void main()
{
	vec3 c = (worldPos[0]+worldPos[1]+worldPos[2])/3; // tocka u centru poligona
	vec3 e = normalize(eye - c); // ociste
	vec3 n = normalize(cross(worldPos[1]-worldPos[0], worldPos[2]-worldPos[0])); // normala poligona
	vec3 l = normalize(light.position-c); // vektor prema svjetlu
	vec3 r = normalize(2*dot(n, l)*n-l);

	vec3 ambient = light.ambientIntensity * material.ka;
	vec3 diffuse = light.intensity * material.kd * dot(l, n);
	vec3 specular = light.intensity * material.ks * pow(max(0, dot(r, e)), material.ksn);
	vec3 color = ambient+diffuse+specular;

	// uklanjanje straznjih poligona
	if (dot(n, e)>0)
	{
		gl_Position = gl_in[0].gl_Position;
		fragColor = color;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		fragColor = color;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		fragColor = color;
		EmitVertex();
		EndPrimitive();
	}
}