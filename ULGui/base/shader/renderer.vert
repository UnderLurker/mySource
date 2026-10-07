#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
uniform mat4 projection;
out vec4 vColor;
out vec2 vUV;
out vec2 vPos;
void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
    vUV = aUV;
    vPos = aPos;
}
