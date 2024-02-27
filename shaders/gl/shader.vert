#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec3 normal;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 cameraMatrix;
uniform mat4 normalMatrix;
uniform vec3 cameraPosition;

out VsOut
{
	vec4 fragPosition;
	vec4 fragColour;
	vec4 fragNormal;
	vec2 textureCoords;
} vsOut;

void main(void)
{
	vsOut.fragPosition = modelMatrix * vec4(position, 1);
	vsOut.fragColour = colour;
	vsOut.fragNormal = normalMatrix * vec4(normal, 0);
	vsOut.textureCoords = textureCoords;

	gl_Position = mvp * vec4(position, 1);
}
