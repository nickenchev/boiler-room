#version 450
#include "header.vert"

layout(location = 0) out vec3 fragPosition;
layout(location = 2) out vec3 fragTexCoord;

void main()
{
	mat4 view = mat4(mat3(rm.view)); // TODO: Move this to CPU

	fragPosition = vertPosition;
	fragTexCoord = vertPosition;
	vec4 pos = rm.projection * view * vec4(vertPosition, 1.0f);
    gl_Position = pos.xyww;
}
