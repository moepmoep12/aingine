#version 430 core
in vec2 TexCoords;
out vec4 color;

layout (location = 4) uniform sampler2D image;
layout (location = 5) uniform vec4 Color;

void main()
{    
    color = Color * texture(image, TexCoords);
} 