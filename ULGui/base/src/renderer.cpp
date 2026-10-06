//
// Created by XW on 2026/10/06.
//
#include "renderer.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <include/logger_wrapper.h>

namespace ULGui::base {

namespace {
// 纯色 + 纹理合一的 shader：纯色时 uv=(0,0)，采样 1x1 白纹理，texture * color = color
const char* const vertexShader = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
uniform mat4 projection;
out vec4 vColor;
out vec2 vUV;
void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
    vUV = aUV;
}
)glsl";

const char* const fragmentShader = R"glsl(
#version 330 core
in vec4 vColor;
in vec2 vUV;
uniform sampler2D tex;
out vec4 FragColor;
void main()
{
    FragColor = texture(tex, vUV) * vColor;
}
)glsl";

constexpr size_t kMaxVertices  = 1u << 16; // 65536 顶点，足够一整屏控件
constexpr int kFloatsPerVertex = 8;        // pos(2) + color(4) + uv(2)
constexpr float kPi            = 3.14159265358979323846f;
} // namespace

Renderer& Renderer::instance() {
    static Renderer renderer;
    return renderer;
}

Renderer::~Renderer() { shutdown(); }

bool Renderer::init(int viewportWidth, int viewportHeight, void* (*getProcAddress)(const char*)) {
    if (_inited) return true;
    if (!gladLoadGLLoader(getProcAddress)) {
        LOGE("[Renderer] glad load failed");
        return false;
    }
    _program = std::make_unique<myUtil::Program>();
    if (!_program) {
        LOGE("[Renderer] create Program fail.");
        return false;
    }

    std::unique_ptr<myUtil::Shader> vShader = std::make_unique<myUtil::VertexShader>(vertexShader);
    std::unique_ptr<myUtil::Shader> fShader = std::make_unique<myUtil::FragmentShader>(fragmentShader);
    if (!vShader || !fShader) return false;
    _program->push_back(std::move(vShader));
    _program->push_back(std::move(fShader));
    if (!_program->linkProgram()) {
        LOGE("[Render] linkProgram fail!!!");
        return false;
    }

    // 1x1 白纹理：让纯色和纹理共用同一个 shader
    unsigned char white[4] = {255, 255, 255, 255};
    _texture = std::make_unique<myUtil::Texture>();
    if (!_texture) {
        LOGE("[Render] create Texture fail!!!");
        return false;
    }
    _texture->use();
    _texture->setImage2D(1, 1, white);
    _texture->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR,
                        GL_TEXTURE_MAG_FILTER, GL_LINEAR,
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // VAO + 动态 VBO（交错布局，预留整帧容量，flush 时 glBufferSubData）
    _vao = std::make_unique<myUtil::VertexArrayObj>();
    if (!_vao) {
        LOGE("[Render] create VertexArrayObj fail!!!");
        return false;
    }
    _vao->setArrayBuffer<float>(kMaxVertices * kFloatsPerVertex, myUtil::VertexArrayObj::DYNAMIC_DRAW);
    _vao->setVertexAttribPointer<float>(2, myUtil::FLOAT, 8, 0);
    _vao->setVertexAttribPointer<float>(4, myUtil::FLOAT, 8, 2);
    _vao->setVertexAttribPointer<float>(2, myUtil::FLOAT, 8, 6);

    glBindVertexArray(0);

    // 2D 不需要深度测试；开启混合支持半透明
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 抗锯齿：多重采样（配合窗口的 GLFW_SAMPLES hint）
    glEnable(GL_MULTISAMPLE);

    _buffer.reserve(kMaxVertices * kFloatsPerVertex);
    setViewport(viewportWidth, viewportHeight);

    _inited = true;
    return true;
}

void Renderer::shutdown() {
    if (!_inited) return;
    _program.reset();
    _texture.reset();
    _vao.reset();
    _inited = false;
}

void Renderer::setViewport(int width, int height) {
    if (width <= 0 || height <= 0) return;
    _viewportW = width;
    _viewportH = height;
    // 左上角原点，y 向下：glm::ortho(0, w, h, 0)
    _projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    if (_inited) glViewport(0, 0, width, height);
}

void Renderer::clear(const RGBA& color) {
    if (!_inited) return;
    glClearColor(color.red(), color.green(), color.blue(), color.alpha());
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::pushVertex(float x, float y, const RGBA& c, float u, float v) {
    _buffer.push_back(x);
    _buffer.push_back(y);
    _buffer.push_back(c.red());
    _buffer.push_back(c.green());
    _buffer.push_back(c.blue());
    _buffer.push_back(c.alpha());
    _buffer.push_back(u);
    _buffer.push_back(v);
    ++_vertexCount;
}

void Renderer::pushTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const RGBA& c) {
    pushVertex(x0, y0, c, 0, 0);
    pushVertex(x1, y1, c, 0, 0);
    pushVertex(x2, y2, c, 0, 0);
}

void Renderer::drawRect(float x, float y, float w, float h, const RGBA& c) {
    float x1 = x + w;
    float y1 = y + h;
    pushTriangle(x, y, x1, y, x1, y1, c);
    pushTriangle(x, y, x1, y1, x, y1, c);
}

void Renderer::drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const RGBA& c) {
    pushTriangle(x0, y0, x1, y1, x2, y2, c);
}

void Renderer::drawLine(float x0, float y0, float x1, float y1, float width, const RGBA& c) {
    if (width <= 0) width = 1.0f;
    float dx  = x1 - x0;
    float dy  = y1 - y0;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1e-6f) return; // 退化线段

    // 垂直于线方向、长度为半宽的偏移，把「线」变成一个细长矩形（2 个三角形）
    float hw = width * 0.5f;
    float nx = -dy / len * hw;
    float ny = dx / len * hw;

    float ax = x0 + nx, ay = y0 + ny;
    float bx = x0 - nx, by = y0 - ny;
    float cx = x1 - nx, cy = y1 - ny;
    float dx_ = x1 + nx, dy_ = y1 + ny;

    pushTriangle(ax, ay, bx, by, cx, cy, c);
    pushTriangle(ax, ay, cx, cy, dx_, dy_, c);
}

void Renderer::pushArc(float cx, float cy, float inner, float outer, float startRad, float endRad, const RGBA& c) {
    // 按半径自适应分段，clamp 到 [16, 256]
    int segments     = static_cast<int>(std::max(1.0f, outer) * 0.5f);
    segments         = std::min(std::max(segments, 16), 256);
    const float step = (endRad - startRad) / static_cast<float>(segments);

    for (int i = 0; i < segments; ++i) {
        float a0  = startRad + i * step;
        float a1  = a0 + step;
        float ca0 = std::cos(a0), sa0 = std::sin(a0);
        float ca1 = std::cos(a1), sa1 = std::sin(a1);

        float ix0 = cx + inner * ca0, iy0 = cy + inner * sa0;
        float ix1 = cx + inner * ca1, iy1 = cy + inner * sa1;
        float ox0 = cx + outer * ca0, oy0 = cy + outer * sa0;
        float ox1 = cx + outer * ca1, oy1 = cy + outer * sa1;

        pushTriangle(ix0, iy0, ox0, oy0, ox1, oy1, c);
        pushTriangle(ix0, iy0, ox1, oy1, ix1, iy1, c);
    }
}

void Renderer::drawCircle(float cx, float cy, float r, const RGBA& c, bool fill, float width) {
    if (r <= 0) return;

    if (fill) {
        // 三角扇：中心 + 圆周
        int segments     = static_cast<int>(r * 0.5f);
        segments         = std::min(std::max(segments, 16), 256);
        const float step = 2.0f * kPi / static_cast<float>(segments);
        for (int i = 0; i < segments; ++i) {
            float a0 = i * step;
            float a1 = a0 + step;
            pushTriangle(cx, cy, cx + r * std::cos(a0), cy + r * std::sin(a0), cx + r * std::cos(a1),
                         cy + r * std::sin(a1), c);
        }
    } else {
        float hw    = (width <= 0 ? 1.0f : width) * 0.5f;
        float inner = std::max(0.0f, r - hw);
        float outer = r + hw;
        pushArc(cx, cy, inner, outer, 0.0f, 2.0f * kPi, c);
    }
}

void Renderer::drawArc(float cx, float cy, float r, float startDeg, float endDeg, float width, const RGBA& c) {
    if (r <= 0 || endDeg <= startDeg) return;
    float hw    = (width <= 0 ? 1.0f : width) * 0.5f;
    float inner = std::max(0.0f, r - hw);
    float outer = r + hw;
    // 角度（度）转弧度
    float startRad = startDeg * kPi / 180.0f;
    float endRad   = endDeg * kPi / 180.0f;
    pushArc(cx, cy, inner, outer, startRad, endRad, c);
}

void Renderer::drawPoint(float x, float y, float size, const RGBA& c) {
    float s = size <= 0 ? 1.0f : size;
    float h = s * 0.5f;
    drawRect(x - h, y - h, s, s, c);
}

void Renderer::flush() {
    if (!_inited || _vertexCount == 0) return;

    _program->use();
    _program->setMatrix4fv("projection", &_projection[0][0]);
    _program->setInt("tex", 0);
    _texture->drawTexture(GL_TEXTURE0);

    _vao->use();
    _vao->updateData(_buffer);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(_vertexCount));
    glBindVertexArray(0);

    _buffer.clear();
    _vertexCount = 0;
}

} // namespace ULGui::base
