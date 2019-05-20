#version 330
uniform mat4 projectionMatrix;
uniform mat4 view;
layout(location = 0) in vec2 v_position;
layout(location = 1) in vec4 v_color;
out vec4 f_color;
void main(void)
{
	f_color = v_color;
	gl_Position = projectionMatrix * view* vec4(v_position, 0.0f, 1.0f);
}