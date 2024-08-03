#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord;

out vec2 texUV;

uniform vec3 size;
uniform mat4 pvm;

void main()
{
    texUV = texCoord;
    gl_Position = pvm * vec4(position * size, 1.0f);
};