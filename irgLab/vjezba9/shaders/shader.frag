#version 330 core

in vec3 fragNorm;
in vec3 fragTang;
in vec3 fragBitang;
in vec2 fragUv;
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
uniform uint flags;

uniform sampler2D diffuseMap;
uniform sampler2D AOMap;
uniform sampler2D specularMap;
uniform sampler2D glossinessMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

const uint HAS_DIFFUSE_MAP = 1u << 0;
const uint HAS_AO_MAP = 1u << 1;
const uint HAS_SPECULAR_MAP = 1u << 2;
const uint HAS_GLOSSINESS_MAP = 1u << 3;
const uint HAS_NORMAL_MAP = 1u << 4;
const uint HAS_HEIGHT_MAP = 1u << 5;

void main()
{
	vec3 n = fragNorm;
	vec3 e = normalize(eye - fragWorldPos); // ociste
	vec3 l = normalize(light.position-fragWorldPos); // vektor prema svjetlu

	// teksture
	vec2 uv = fragUv;
	if ((flags & HAS_NORMAL_MAP)!=0u || (flags & HAS_HEIGHT_MAP)!=0u){
		mat3 nbt = mat3(fragTang, fragBitang, fragNorm);

		if ((flags & HAS_HEIGHT_MAP)!=0u){
			vec3 viewTangent = normalize(transpose(nbt) * e);
			float height = texture(heightMap, uv).r;
			uv += viewTangent.xy * (1-height) * 0.005;
		}
		if ((flags & HAS_NORMAL_MAP)!=0u){
			vec3 norm = texture(normalMap, uv).rgb * 2.0 - 1.0;
			n = normalize(nbt * norm);
		}
	}
	float shininess = material.ksn;
	if ((flags & HAS_GLOSSINESS_MAP)!=0u){
		shininess *= texture(glossinessMap, uv).r;
	}

	vec3 r = normalize(2*dot(n, l)*n-l);

	// materijali
	vec3 ambient = light.ambientIntensity * material.ka;
	vec3 diffuse = light.intensity * material.kd * max(0, dot(l, n));
	vec3 specular = vec3(0,0,0);
	if(material.ksn>0 && dot(l, n)>0){
		specular = light.intensity * material.ks * pow(max(0, dot(r, e)), shininess);
	}

	// teksture
	if ((flags & HAS_DIFFUSE_MAP)!=0u){
		diffuse = light.intensity * texture(diffuseMap, uv).rgb * max(0, dot(l, n));
		ambient *= texture(diffuseMap, uv).rgb;
	}
	if ((flags & HAS_AO_MAP)!=0u){
		ambient *= texture(AOMap, uv).r;
	}
	if ((flags & HAS_SPECULAR_MAP)!=0u){
		specular *= texture(specularMap, uv).rgb;
	}

	vec3 color = ambient+diffuse+specular;
    FragColor = vec4(color, 1.0);
} 