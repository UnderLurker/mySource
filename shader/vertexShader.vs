#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 point;

out vec4 vertexColor;
out vec2 coord;

uniform float offset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4(aPos, 1.0);
    //vertexColor = vec4(color, 1);
    coord = point;
}