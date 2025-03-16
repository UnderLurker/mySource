#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 vertexColor;
out vec3 normal;
out vec3 FragPos;

uniform float offset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4(aPos, 1.0);
    //vertexColor = vec4(color, 1);
    FragPos = vec3(model * vec4(aPos, 1.0));
    normal = aNormal;
}