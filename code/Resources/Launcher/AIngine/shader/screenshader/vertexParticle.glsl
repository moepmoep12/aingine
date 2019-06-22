#version 430 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec4 Color;
layout (location = 2) in vec2 Offset;
layout (location = 3) in vec2 Size;
layout (location = 4) in float Rotation;

layout (location = 5) uniform mat4 model;
layout (location = 6) uniform mat4 projection;
layout (location = 7) uniform mat4 view;

out vec2 TexCoords;
out vec4 fColor;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    TexCoords = vertex.zw;
	vec4 worldPos = model * rotationMatrix(vec3(0,0,1), Rotation) * vec4(vertex.x * Size.x , vertex.y * Size.y, 0.0,1.0);
	worldPos += vec4(Offset.xy,0,0);
	
    gl_Position = projection * view * worldPos;
	fColor = Color;
}
