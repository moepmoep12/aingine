#version 430 core
in vec2 TexCoords;
out vec4 color;

layout (location = 4) uniform sampler2D image;
layout (location = 5) uniform vec3 spriteColor;

void main()
{    
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
} 