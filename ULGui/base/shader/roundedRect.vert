#version 330 core
layout (location = 0) in vec2 aCorner;   // 单位 quad 顶点 (0/1)，divisor 0，静态 VBO
layout (location = 1) in vec4 aRect;     // x, y, w, h           — divisor 1
layout (location = 2) in vec4 aRadii;    // lt, rt, rb, lb       — divisor 1
layout (location = 3) in vec4 aBorder;   // wL, wT, wR, wB       — divisor 1
layout (location = 4) in vec4 aColor;    // r, g, b, a (0..1)    — divisor 1
uniform mat4 projection;

out vec2 vLocal;      // 矩形局部坐标 [0,w]x[0,h]（y 向下）
out vec2 vRectSize;
out vec4 vRadii;
out vec4 vBorder;
out vec4 vColor;
out vec2 vPos;        // 世界像素坐标（渐变采样，y 向下）

void main() {
    vLocal      = aCorner * aRect.zw;
    vRectSize   = aRect.zw;
    vRadii      = aRadii;
    vBorder     = aBorder;
    vColor      = aColor;
    vPos        = aRect.xy + vLocal;
    gl_Position = projection * vec4(aRect.xy + vLocal, 0.0, 1.0);
}
