#version 330 core
in vec2 vLocal;
in vec2 vRectSize;
in vec4 vRadii;
in vec4 vBorder;
in vec4 vColor;
in vec2 vPos;

// 渐变 uniform 块与 renderer.frag 完全一致，uploadBrushUniforms 两个 program 共用
uniform int uBrushType;    // 0 纯色，1 线性渐变，2 径向渐变
uniform int uSpread;       // 0 pad，1 repeat，2 reflect
uniform vec2 uGradStart;
uniform vec2 uGradStop;
uniform vec2 uGradCenter;
uniform float uGradRadius;
uniform int uStopCount;
uniform float uStopPos[16];
uniform vec4 uStopColor[16];

out vec4 FragColor;

float applySpread(float t) {
    if (uSpread == 1) return mod(t, 1.0);        // repeat
    if (uSpread == 2) {                          // reflect
        float f = mod(abs(t), 2.0);
        return f > 1.0 ? 2.0 - f : f;
    }
    return clamp(t, 0.0, 1.0);                   // pad
}

vec4 sampleStops(float t) {
    if (uStopCount <= 0) return vec4(1.0);
    float pos = applySpread(t);
    if (pos <= uStopPos[0]) return uStopColor[0];
    for (int i = 1; i < uStopCount; ++i) {
        if (pos <= uStopPos[i]) {
            float span = uStopPos[i] - uStopPos[i - 1];
            float ratio = span <= 0.0 ? 0.0 : (pos - uStopPos[i - 1]) / span;
            return mix(uStopColor[i - 1], uStopColor[i], ratio);
        }
    }
    return uStopColor[uStopCount - 1];
}

// 投影 y 向下：p.y > 0 为下半。按象限选角半径 r = (lt, rt, rb, lb)
float cornerRadius(vec2 p, vec4 r) {
    return p.x > 0.0 ? (p.y > 0.0 ? r.z : r.y) : (p.y > 0.0 ? r.w : r.x);
}

// 圆角盒 SDF（iq 公式）：q = |p| - (half - r)，圆心 half - r，弧半径 r。
// 外盒半径由 CPU 侧 clamp 到 [0, min(half)]；内盒的 rInner 在下方再 clamp 一次。
float sdRoundBox(vec2 p, vec2 halfSize, vec4 r) {
    float rr = cornerRadius(p, r);
    vec2 q = abs(p) - (halfSize - vec2(rr));
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - rr;
}

void main() {
    vec2 p = vLocal - 0.5 * vRectSize;

    // 外边界
    float d  = sdRoundBox(p, 0.5 * vRectSize, vRadii);
    float aa = max(fwidth(d), 1e-4);
    float alpha = 1.0 - smoothstep(-aa, aa, d);
    if (alpha <= 0.0) discard;

    float wL = vBorder.x, wT = vBorder.y, wR = vBorder.z, wB = vBorder.w;
    bool stroke = wL > 0.0 || wT > 0.0 || wR > 0.0 || wB > 0.0;

    vec4 base = vColor;
    if (!stroke) {
        if (uBrushType == 1) {
            vec2 dGrad = uGradStop - uGradStart;
            float len2 = dot(dGrad, dGrad);
            float t = len2 <= 0.0 ? 0.0 : dot(vPos - uGradStart, dGrad) / len2;
            base = sampleStops(t);
        } else if (uBrushType == 2) {
            float t = uGradRadius <= 0.0 ? 0.0 : length(vPos - uGradCenter) / uGradRadius;
            base = sampleStops(t);
        }
    } else {
        // 描边 = 外盒 \ 各边内缩后的内盒（圆角环带）
        vec2 offset = 0.5 * vec2(wL - wR, wT - wB);
        vec2 halfInner = max(0.5 * vRectSize - 0.5 * vec2(wL + wR, wT + wB), vec2(0.0));
        float maxR = max(min(halfInner.x, halfInner.y), 0.0);
        // 内角半径 = 外半径 - 相邻两边平均宽；clamp 到内盒半宽/半高，
        // 否则 halfInner - rInner < 0 会让 SDF 退化（大圆角 + 不等边宽时会发生）
        vec4 rInner = clamp(vRadii - 0.5 * vec4(wL + wT, wT + wR, wR + wB, wB + wL), 0.0, maxR);
        if (halfInner.x > 0.0 && halfInner.y > 0.0) {
            float dInner = sdRoundBox(p - offset, halfInner, rInner);
            alpha *= smoothstep(-aa, aa, dInner);
        }
        // halfInner 塌缩（线宽吞掉矩形）时不挖内孔，保留整体填充 —— 与旧实现（边矩形并集）一致
    }

    // 直通 alpha，配合 GL_SRC_ALPHA / GL_ONE_MINUS_SRC_ALPHA 混合做边缘抗锯齿
    FragColor = vec4(base.rgb, base.a * alpha);
}
