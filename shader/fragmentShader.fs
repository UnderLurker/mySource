#version 330 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 coord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
//    FragColor = vertexColor;
    FragColor = mix(texture(texture1, coord), texture(texture2, coord), 0.2);
}