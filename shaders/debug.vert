#version 450
#include "header.vert"

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

void main()
{
	mat4 modelMatrix = matrices.data[constants.matrixId];
	
	vec3 offsetPosition = vertPosition + vec3(constants.offset);
    gl_Position = rm.viewProjection * modelMatrix * vec4(offsetPosition, 1.0f);

	fragPosition = vec3(modelMatrix * vec4(offsetPosition, 1.0));
	fragColor = vertColor;
	fragTexCoord = vertTexCoord;
	fragNormal = mat3(transpose(inverse(modelMatrix))) * vertNormal; // TODO: Move the inversion to the CPU
}
