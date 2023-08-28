#version 450 core

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;

layout (location = 2) uniform vec3 cameraPosition;

in VsOut
{
	vec4 fragPosition;
	vec4 fragColour;
	vec4 fragNormal;
	vec4 textureCoords;
} fsIn;

out vec4 outColour;

vec3 lit(vec3 l, vec3 n, vec3 v)
{
	vec3 reflectL = reflect(-l, n);
	float s = clamp(100.0 * dot(reflectL, v) - 97.0, 0, 1);
	vec3 warmColour = vec3(0.3, 0.3, 0);
	vec3 highlightColour = vec3(2, 2, 2);

	return mix(warmColour, highlightColour, s);
}

void main(void)
{
	vec4 fragAlbedo = texture(albedoSampler, fsIn.textureCoords.xy);
	vec4 fragNormal = texture(normalSampler, fsIn.textureCoords.xy);

	vec3 lightPosition = vec3(-5, 10, 0);
	vec3 lightColour = vec3(1, 1, 1);

//	float ambientStrength = 0.2;
//	vec3 ambientColour = vec3(1, 1, 1);
//	vec3 ambient = ambientStrength * ambientColour;
//
//	vec3 normal = normalize(vec3(fsIn.fragNormal));
//
//	vec3 lightDirection = normalize(lightPosition - vec3(fsIn.fragPosition));
//	float diff = max(dot(normal, lightDirection), 0);
//	vec3 diffuse = diff * lightColour;
//
//	float specularStrength = 1;
//	vec3 viewDir = normalize(cameraPosition - vec3(fsIn.fragPosition));
//	vec3 reflectDir = reflect(-lightDirection, normal);
//
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//	vec3 specular = specularStrength * spec * lightColour;
//
//	vec4 result = vec4(ambient + diffuse + specular, 1.0) * fragAlbedo;
//	outColour = vec4(result.r, result.g, result.b, 1);

	vec3 n = normalize(fragNormal.xyz);
	vec3 v = normalize(cameraPosition - fsIn.fragPosition.xyz);

	// per light calculation
	vec3 l = normalize(lightPosition - fsIn.fragPosition.xyz);
	float Ndl = clamp(dot(n, l), 0, 1);

	outColour = fragAlbedo;
	outColour.rgb += Ndl * lightColour * lit(l, n, v);
}
