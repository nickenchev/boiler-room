#version 450
#include "header.frag"

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outAlbedo;
layout(location = 2) out vec4 outNormal;

void main()
{
	Material material = materials.data[constants.materialId];
	
	outAlbedo = fragColor * material.baseColorFactor;
	if (USE_TEXTURE)
	{
		outAlbedo *= texture(baseTexSampler, fragTexCoord);
	}

	outPosition = vec4(fragPosition, 1);
	outNormal = vec4(fragNormal, 1);
}

