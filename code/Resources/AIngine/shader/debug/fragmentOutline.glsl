#version 330
uniform vec4 outlinecolor = vec4(0.706, 1.0, 0.2, 0.6);
uniform float time = 3.55f;
out vec4 color;

void main(void)
{
	color = outlinecolor;
	color.a =sin(time) + 1.0f;
}