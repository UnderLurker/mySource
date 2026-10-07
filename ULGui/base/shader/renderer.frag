#version 330 core
in vec4 vColor;
in vec2 vUV;
in vec2 vPos;
uniform sampler2D tex;

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

void main()
{
    vec4 baseColor = vColor;
    if (uBrushType == 1) {
        vec2 d = uGradStop - uGradStart;
        float len2 = dot(d, d);
        float t = len2 <= 0.0 ? 0.0 : dot(vPos - uGradStart, d) / len2;
        baseColor = sampleStops(t);
    } else if (uBrushType == 2) {
        float t = uGradRadius <= 0.0 ? 0.0 : length(vPos - uGradCenter) / uGradRadius;
        baseColor = sampleStops(t);
    }
    FragColor = texture(tex, vUV) * baseColor;
}
