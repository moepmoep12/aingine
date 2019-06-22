#version 430 core
in vec2 TexCoords;
in vec4 fColor;
out vec4 color;

layout (location = 8) uniform sampler2D image;

void main()
{    
    color = fColor * texture(image, TexCoords);
} 