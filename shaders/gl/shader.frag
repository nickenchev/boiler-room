#version 450 core

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;
layout (binding = 2) uniform sampler2D metallicRoughSampler;

struct Material
{
	sampler2D albedoSampler;
};

struct Light
{
	vec4 position;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	uint lightType;
};

uniform vec3 cameraPosition;

layout(std140) uniform Lighting
{
	uint frameLights;
	Light lights[10];
} lighting;

in VsOut
{
	vec4 fragPosition;
	vec4 fragColour;
	vec4 fragNormal;
	vec2 textureCoords;
} fsIn;

//out vec4 outColour;

layout (location = 0) out vec4 fboColor;

void main(void)
{
	vec4 albedo = texture(albedoSampler, fsIn.textureCoords.xy);
	vec4 normal = texture(normalSampler, fsIn.textureCoords.xy);
	vec4 metalRough = texture(metallicRoughSampler, fsIn.textureCoords.xy);
	vec4 fsNormal = normalize(fsIn.fragNormal);
	vec4 lightAmount = vec4(0);

	for (int i = 0; i < lighting.frameLights; ++i)
	{
		// ambient and diffuse
		float ambientStrength = 0.1;
		vec4 ambient = lighting.lights[i].ambient * ambientStrength;

		if (lighting.lights[i].lightType == 1)
		{
			// diffuse
			vec4 lightDir = normalize(lighting.lights[i].position - fsIn.fragPosition);
			lightDir = -vec4(normalize(vec3(-1, -1, 0)), 0);
			float diffuseFactor = max(dot(fsNormal, lightDir), 0.0);
			vec4 diffuse = lighting.lights[i].diffuse * diffuseFactor;

			// specular highlight
			vec4 reflectDir = reflect(-lightDir, fsNormal);
			vec4 viewDir = normalize(vec4(cameraPosition, 1) - fsIn.fragPosition);
			float specularStrength = 0.5;
			float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), 64);
			vec4 specular = specularStrength * specularFactor * lighting.lights[i].specular;
			lightAmount += ambient + diffuse + specular;
		}
		if (lighting.lights[i].lightType == 2)
		{
			// diffuse
			vec4 lightDir = normalize(lighting.lights[i].position - fsIn.fragPosition);
			float diffuseFactor = max(dot(fsNormal, lightDir), 0.0);
			vec4 diffuse = lighting.lights[i].diffuse * diffuseFactor;

			// specular highlight
			vec4 reflectDir = reflect(-lightDir, fsNormal);
			vec4 viewDir = normalize(vec4(cameraPosition, 1) - fsIn.fragPosition);
			float specularStrength = 0.5;
			float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), 64);
			vec4 specular = specularStrength * specularFactor * lighting.lights[i].specular;
			lightAmount += ambient + diffuse + specular;
		}
	}
	//outColour = lightAmount * albedo;
	fboColor = lightAmount * albedo;
}
