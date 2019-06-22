#version 430 core
//from learnopengl.com
in vec2 TexCoords;
out vec4 color;

layout(location = 2) uniform sampler2D text;
layout(location = 3) uniform vec3 textColor;
layout(location = 4) uniform float alpha = 1.0f;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, alpha) * sampled;
} 