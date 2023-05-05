#version 450 core

in VsOut
{
	vec4 fragColor;
} fsIn;
out vec4 color;

void main(void)
{
	color = fsIn.fragColor;
}