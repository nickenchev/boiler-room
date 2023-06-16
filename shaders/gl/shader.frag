#version 450 core

layout (binding = 0) uniform sampler2D baseSampler;

in VsOut
{
	vec4 fragColour;
	vec4 textureCoords;
} fsIn;

out vec4 colour;

void main(void)
{
	colour = fsIn.fragColour;
	colour = texture(baseSampler, fsIn.textureCoords.xy);
}