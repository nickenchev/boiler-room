#version 450 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) uniform mat4 mv;

out VsOut
{
	vec4 fragColor;
} vsOut;

void main(void)
{
	gl_Position = mv * position;
	vsOut.fragColor = vec4(position.z, position.z, position.z, 1) + 0.3;
}