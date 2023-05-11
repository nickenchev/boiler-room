#version 450 core

in VsOut
{
	vec4 fragColour;
} fsIn;
out vec4 colour;

void main(void)
{
	colour = fsIn.fragColour;
}