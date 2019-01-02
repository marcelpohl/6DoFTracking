#version 460 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	bool hasDiffuseTex;
	sampler2D diffuseTex;
	bool hasSpecularTex;
	sampler2D specularTex;
	bool hasAmbientOcclusionTex;
	sampler2D ambientOcclusionTex;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 xTexCoord;
in vec3 xNormal;
in vec3 xFragPos;

out vec4 FragColor;

uniform Material mat;
uniform int numLights;
uniform Light lights[4];
uniform vec3 camPos;

vec3 calcLight(Material mat, Light light, vec3 normal, vec3 viewDir, vec3 fragPos, vec2 texCoord)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	
	// ambient lighting
	vec3 ambient;
	if (mat.hasDiffuseTex) {
		ambient = light.ambient * texture(mat.diffuseTex, texCoord).xyz;
	} else {
		ambient = light.ambient * mat.ambient;
	}
	
	// diffuse lighting
	vec3 diffuseLight = max(dot(normal, lightDir), 0.0) * light.diffuse;
	vec3 diffuse;
	if (mat.hasDiffuseTex) {
		diffuse = diffuseLight * texture(mat.diffuseTex, texCoord).xyz;
	} else {
		diffuse = diffuseLight * mat.diffuse;
	}
	
	// specular lighting
	float specVal = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	vec3 specular ;
	if (mat.hasSpecularTex) {
		specular = light.specular * specVal * texture(mat.specularTex, texCoord).xyz;
	} else {
		specular = light.specular * specVal * mat.specular;
	}
	
	// output color
	return ambient + diffuse + specular;
}

void main()
{
	// setup variables
	vec3 normal = normalize(xNormal);
	vec3 viewDir = normalize(camPos - xFragPos);
	
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < numLights; i++) 
	{
		color += calcLight(mat, lights[i], normal, viewDir, xFragPos, xTexCoord);
	}
	
	FragColor = vec4(color, 1.0f);
} 