#version 450
#include "header.frag"

layout(location = 0) out vec4 outColor;

void main()
{
	Material material = materials.data[constants.materialId];
	vec4 tex = texture(baseTexSampler, fragTexCoord);

	if (tex.r + tex.g + tex.b == 0) discard;
	outColor = material.baseColorFactor * fragColor;
}

