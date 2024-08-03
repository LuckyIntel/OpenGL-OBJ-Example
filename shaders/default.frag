#version 330 core

out vec4 FragColor;
in vec2 texUV;

uniform vec3 color;
uniform sampler2D tex_d1;

void main()
{
    if (color == vec3(0.0f)) FragColor = texture(tex_d1, texUV);
    else FragColor = vec4(color, 1.0f);
};