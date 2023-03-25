#version 450
#include "header.vert"

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

void main()
{
	mat4 modelMatrix = matrices.data[constants.matrixId];
	
	vec4 worldVert = modelMatrix * vec4(vertPosition, 1.0);
	fragPosition = worldVert.xyz;
	fragColor = vertColor;
	fragTexCoord = vertTexCoord;
	fragNormal = mat3(transpose(inverse(modelMatrix))) * vertNormal; // TODO: Move the inversion to the CPU
    gl_Position = rm.viewProjection * worldVert;
}
