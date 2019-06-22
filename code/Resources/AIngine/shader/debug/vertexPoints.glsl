#version 430
layout(location = 0) in vec2 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_size;
layout(location = 3) uniform mat4 projectionMatrix;
layout(location = 4) uniform mat4 view;

out vec4 f_color;

void main(void)
{
	f_color = v_color;
	gl_Position = projectionMatrix * view * vec4(v_position, 0.0f, 1.0f);
   gl_PointSize = v_size;
}