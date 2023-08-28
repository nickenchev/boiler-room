#version 450 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec4 textureCoords;
layout (location = 3) in vec4 normal;

layout (location = 0) uniform mat4 mvp;
layout (location = 1) uniform mat4 modelMatrix;
layout (location = 2) uniform vec3 cameraPosition;

out VsOut
{
	vec4 fragPosition;
	vec4 fragColour;
	vec4 fragNormal;
	vec4 textureCoords;
} vsOut;

void main(void)
{
	vsOut.fragPosition = modelMatrix * position;
	vsOut.fragColour = colour;
	vsOut.fragNormal = modelMatrix * normal;
	vsOut.textureCoords = textureCoords;
	gl_Position = mvp * position;
}
