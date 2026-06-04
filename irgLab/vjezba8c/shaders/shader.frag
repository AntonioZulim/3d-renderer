#version 330 core

in vec3 fragNorm;
in vec3 fragWorldPos;
out vec4 FragColor;

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
	vec3 n = fragNorm;
	vec3 e = normalize(eye - fragWorldPos); // ociste
	vec3 l = normalize(light.position-fragWorldPos); // vektor prema svjetlu
	vec3 r = normalize(2*dot(n, l)*n-l);

	vec3 ambient = light.ambientIntensity * material.ka;
	vec3 diffuse = light.intensity * material.kd * max(0, dot(l, n));
	vec3 specular = vec3(0,0,0);
	if(material.ksn>0)
		specular = light.intensity * material.ks * pow(max(0, dot(r, e)), material.ksn);
	vec3 color = ambient+diffuse+specular;
    FragColor = vec4(color, 1.0);
} 