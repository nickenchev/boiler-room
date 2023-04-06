#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inPositions;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inAlbedos;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inNormals;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput inDepths;

struct LightSource
{
	vec4 position;
	vec4 color;
};

layout(binding = 3) uniform Lights
{
	LightSource sources[64];
} lights;

layout(push_constant) uniform Constants
{
	vec3 cameraPosition;
} constants;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 fragPosition = subpassLoad(inPositions);
	vec4 fragAlbedo = subpassLoad(inAlbedos);
	vec4 fragNormal = subpassLoad(inNormals);
	
	vec3 lightPosition = vec3(lights.sources[0].position);
	vec3 lightColour = vec3(lights.sources[0].color);

	float ambientStrength = 0.05;
	vec3 ambientColour = vec3(1, 1, 1);
	vec3 ambient = ambientStrength * ambientColour;

	vec3 normal = normalize(vec3(fragNormal));

	vec3 lightDirection = normalize(lightPosition - vec3(fragPosition));
	//vec3 lightDirection = normalize(lightPosition);
	float diff = max(dot(normal, lightDirection), 0);
	vec3 diffuse = diff * lightColour;

	float specularStrength = 1;
	vec3 viewDir = normalize(constants.cameraPosition - vec3(fragPosition));
	vec3 reflectDir = reflect(-lightDirection, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColour;

	vec4 result = vec4(ambient + diffuse + specular, 1.0) * fragAlbedo;
	outColor = vec4(result.r, result.g, result.b, 1);
}

