#version 450 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec4 textureCoords;
layout (location = 3) in vec4 normal;

layout (location = 0) uniform mat4 mvp;

out VsOut
{
	vec4 fragColour;
} vsOut;

void main(void)
{
	gl_Position = mvp * position;
	vsOut.fragColour = vec4(position.z, position.z, position.z, 1) + 0.3;
}