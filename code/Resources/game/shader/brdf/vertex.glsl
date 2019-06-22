#version 410  

// Attributes
in vec3         position;
in vec3         normal;
in vec2         texCoord;
in vec4         tangent;

// Interpolated outputs
out Varying{
    vec3        normal;
    vec2        texCoord;
    vec3        position;
    vec3        tangent;
    flat float  tangentYSign;
} vertexOutput;

// Uniforms
layout(std140) uniform Uniform{
    vec4       light;
    vec4       cameraPosition;
} object;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 objectToWorldNormalMatrix;
uniform mat4 objectToWorldMatrix;

void main() {
    vertexOutput.texCoord = texCoord;
    // TODO objectToWorldNormalMatrix is not being passed correctly
    vertexOutput.normal = normalize(mat3x3(objectToWorldNormalMatrix) * normal);
    vertexOutput.tangent = normalize(mat3x3(objectToWorldMatrix) * tangent.xyz);
    vertexOutput.tangentYSign = tangent.w;
    vertexOutput.position = (objectToWorldMatrix * vec4(position, 1.0)).xyz;

    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
}
