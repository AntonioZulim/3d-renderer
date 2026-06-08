#version 330 core

#define MAX_LIGHTS 10

in vec3 fragNorm;
in vec3 fragTang;
in vec3 fragBitang;
in vec2 fragUv;
in vec3 fragWorldPos;
in vec4 fragLightPos[MAX_LIGHTS];
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 intensity;
    vec3 ambientIntensity;
	vec3 direction;
	float cutoff;
};

struct Material {
	vec3 ka, kd, ks;
    float ksn;
};

uniform vec3 eye;
uniform Light lights[MAX_LIGHTS];
uniform uint numLights;
uniform Material material;
uniform uint flags;

uniform sampler2D diffuseMap;
uniform sampler2D AOMap;
uniform sampler2D specularMap;
uniform sampler2D glossinessMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform sampler2D shadowMaps[MAX_LIGHTS];

const uint HAS_DIFFUSE_MAP = 1u << 0;
const uint HAS_AO_MAP = 1u << 1;
const uint HAS_SPECULAR_MAP = 1u << 2;
const uint HAS_GLOSSINESS_MAP = 1u << 3;
const uint HAS_NORMAL_MAP = 1u << 4;
const uint HAS_HEIGHT_MAP = 1u << 5;

float calculateShadow(vec4 lightPos, sampler2D shadowMap) {
	vec3 proj = lightPos.xyz / lightPos.w;
	proj = (proj + 1.0f) / 2.0f;
    vec2 center = proj.xy - vec2(0.5);
	if (proj.z > 1.0 || length(center) > 0.5)
        return 0.0;
   	
    float closestDepth = texture(shadowMap, proj.xy).r;
    float currentDepth = proj.z;
    float bias = 0.005;
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

vec3 calculateLight(Light light, vec3 n, vec3 e, vec2 uv, float shininess, uint index)
{
	vec3 l = normalize(light.position-fragWorldPos); // vektor prema svjetlu
	float theta = dot(l, -normalize(light.direction));

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

	if (light.cutoff!=-1.0f && theta<light.cutoff){
		return ambient;
	}

	float shadow = 0;
	if (light.cutoff != -1.0f) {
		shadow = calculateShadow(fragLightPos[index], shadowMaps[index]);
	}
	return ambient + (diffuse+specular) * (1-shadow);
}

void main(){
	vec3 n = fragNorm;
	vec3 e = normalize(eye - fragWorldPos); // ociste

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

	vec3 color = vec3(0,0,0);
	for (uint i = 0u; i<numLights; i++){
		color += calculateLight(lights[i], n, e, uv, shininess, i);
	}
	FragColor = vec4(color, 1.0);
}