#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

out vec3 worldPos;
out vec3 color;

struct Light {
    vec3 position;
    vec3 intensity;
    vec3 ambientIntensity;
};

struct Material {
	vec3 ka, kd, ks;
    float ksn;
};

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 perspectiveMat;
uniform mat4 normalMat;

uniform vec3 eye;
uniform Light light;
uniform Material material;

void main()
{
    worldPos = vec3(modelMat * vec4(vPos, 1.0));
    gl_Position = perspectiveMat * viewMat * vec4(worldPos, 1.0);

	vec3 n = normalize(vec3(normalMat * vec4(vNorm, 0)));
	vec3 e = normalize(eye - worldPos); // ociste
	vec3 l = normalize(light.position-worldPos); // vektor prema svjetlu
	vec3 r = normalize(2*dot(n, l)*n-l);

	vec3 ambient = light.ambientIntensity * material.ka;
	vec3 diffuse = light.intensity * material.kd * max(0, dot(l, n));
	vec3 specular = vec3(0,0,0);
	if(material.ksn>0)
		specular = light.intensity * material.ks * pow(max(0, dot(r, e)), material.ksn);
	color = ambient+diffuse+specular;
}