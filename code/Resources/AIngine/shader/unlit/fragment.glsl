#version 410 core

in Varying{
    vec3 normal;
    vec2 texCoord;
    vec3 position;
    vec3 tangent;
    flat float tangentYSign;
} interpolated;

uniform sampler2D colorTexture;
out vec3 pixelColor;

void main() {
    pixelColor = texture(colorTexture, interpolated.texCoord).rgb;
    //pixelColor = vec4(interpolated.texCoord.xy, 0, 1);
	//pixelColor = vec3(1, 0, 0);
}
