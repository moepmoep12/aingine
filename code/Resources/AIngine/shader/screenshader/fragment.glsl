#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = vec4(texture(screenTexture, TexCoords).xyz, 1);
    //FragColor = vec4(TexCoords.xy, 0, 1);
}

