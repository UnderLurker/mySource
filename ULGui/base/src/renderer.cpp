//
// Created by XW on 2026/10/06.
//
#include "renderer.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <include/logger_wrapper.h>

#include "border.h"

namespace ULGui::base {

namespace {
constexpr size_t kMaxVertices   = 1u << 16; // 65536 顶点，足够一整屏控件
constexpr int kFloatsPerVertex  = 8;        // pos(2) + color(4) + uv(2)
constexpr float kPi             = 3.14159265358979323846f;
constexpr int kMaxGradientStops = 16; // shader 中颜色 stop 数组的上限

// 虚线/点线图案比例（相对线宽）
constexpr float kDashLengthScale  = 3.0f; // 实段 = 线宽 * 3
constexpr float kGapLengthScale   = 2.0f; // 空段 = 线宽 * 2
constexpr float kDotLengthScale   = 0.9f; // 点线实段 ≈ 线宽
constexpr int kMaxDashArcSegments = 512;  // 每段角弧最多细分段数（约 1px/段，防顶点缓冲溢出）

// 顶点色只承载纯色；渐变画刷在 fragment shader 中逐像素计算，这里填白色占位
RGBA vertexColor(const ULBrush& brush) {
    const auto style = brush.style();
    if (style == BrushStyle::LinearGradient || style == BrushStyle::RadialGradient) return RGBA(255, 255, 255, 255);
    return brush.color();
}
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

    std::unique_ptr<myUtil::Shader> vShader =
        std::make_unique<myUtil::VertexShader>(std::string("base/shader/renderer.vert"));
    std::unique_ptr<myUtil::Shader> fShader =
        std::make_unique<myUtil::FragmentShader>(std::string("base/shader/renderer.frag"));
    if (!vShader || !fShader) return false;
    _program->push_back(std::move(vShader));
    _program->push_back(std::move(fShader));
    if (!_program->linkProgram()) {
        LOGE("[Render] linkProgram fail!!!");
        return false;
    }

    // 1x1 白纹理：让纯色和纹理共用同一个 shader
    unsigned char white[4] = {255, 255, 255, 255};
    _texture               = std::make_unique<myUtil::Texture>();
    if (!_texture) {
        LOGE("[Render] create Texture fail!!!");
        return false;
    }
    _texture->use();
    _texture->setImage2D(1, 1, white);
    _texture->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR, GL_TEXTURE_MAG_FILTER, GL_LINEAR, GL_TEXTURE_WRAP_S,
                       GL_CLAMP_TO_EDGE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
    _buffer.push_back((float)c.red() / 255.0f);
    _buffer.push_back((float)c.green() / 255.0f);
    _buffer.push_back((float)c.blue() / 255.0f);
    _buffer.push_back((float)c.alpha() / 255.0f);
    _buffer.push_back(u);
    _buffer.push_back(v);
    ++_vertexCount;
}

void Renderer::pushTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const ULBrush& brush) {
    applyBrush(brush);
    const RGBA color = vertexColor(brush);
    pushVertex(x0, y0, color, 0, 0);
    pushVertex(x1, y1, color, 0, 0);
    pushVertex(x2, y2, color, 0, 0);
}

void Renderer::drawRect(float x, float y, float w, float h, const ULBrush& brush) {
    float x1 = x + w;
    float y1 = y + h;
    pushTriangle(x, y, x1, y, x1, y1, brush);
    pushTriangle(x, y, x1, y1, x, y1, brush);
}

void Renderer::drawRoundedRectFill(float x, float y, float w, float h, float radius, const ULBrush& brush) {
    if (w <= 0 || h <= 0) return;
    // 圆角半径 clamp 到矩形可容纳的范围
    const float r = std::clamp(std::max(0.0f, radius), 0.0f, 0.5f * std::min(w, h));
    if (r <= 0.0f) {
        drawRect(x, y, w, h, brush);
        return;
    }

    drawRect(x + r, y, w - 2.0f * r, r, brush);                 // 上
    drawRect(x, y + r, r, h - 2.0f * r, brush);                 // 左
    drawRect(x + w - r, y + r, r, h - 2.0f * r, brush);         // 右
    drawRect(x + r, y + h - r, w - 2.0f * r, r, brush);         // 下
    drawRect(x + r, y + r, w - 2.0f * r, h - 2.0f * r, brush);  // 中

    // 角度约定同 drawBorder：0° 朝右、90° 朝下（y 向下）
    pushArc(x + r, y + r, 0.0f, r, kPi, 1.5f * kPi, brush);                 // 左上 180°~270°
    pushArc(x + w - r, y + r, 0.0f, r, 1.5f * kPi, 2.0f * kPi, brush);      // 右上 270°~360°
    pushArc(x + w - r, y + h - r, 0.0f, r, 0.0f, 0.5f * kPi, brush);        // 右下 0°~90°
    pushArc(x + r, y + h - r, 0.0f, r, 0.5f * kPi, kPi, brush);             // 左下 90°~180°
}

void Renderer::drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const ULBrush& brush) {
    pushTriangle(x0, y0, x1, y1, x2, y2, brush);
}

void Renderer::drawLine(float x0, float y0, float x1, float y1, float width, const ULBrush& brush) {
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

    pushTriangle(ax, ay, bx, by, cx, cy, brush);
    pushTriangle(ax, ay, cx, cy, dx_, dy_, brush);
}

void Renderer::pushArc(float cx,
                       float cy,
                       float inner,
                       float outer,
                       float startRad,
                       float endRad,
                       const ULBrush& brush) {
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

        pushTriangle(ix0, iy0, ox0, oy0, ox1, oy1, brush);
        pushTriangle(ix0, iy0, ox1, oy1, ix1, iy1, brush);
    }
}

void Renderer::drawCircle(float cx, float cy, float r, const ULBrush& brush, bool fill, float width) {
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
                         cy + r * std::sin(a1), brush);
        }
    } else {
        float hw    = (width <= 0 ? 1.0f : width) * 0.5f;
        float inner = std::max(0.0f, r - hw);
        float outer = r + hw;
        pushArc(cx, cy, inner, outer, 0.0f, 2.0f * kPi, brush);
    }
}

void Renderer::drawArc(float cx, float cy, float r, float startDeg, float endDeg, float width, const ULBrush& brush) {
    if (r <= 0 || endDeg <= startDeg) return;
    float hw    = (width <= 0 ? 1.0f : width) * 0.5f;
    float inner = std::max(0.0f, r - hw);
    float outer = r + hw;
    // 角度（度）转弧度
    float startRad = startDeg * kPi / 180.0f;
    float endRad   = endDeg * kPi / 180.0f;
    pushArc(cx, cy, inner, outer, startRad, endRad, brush);
}

void Renderer::drawPoint(float x, float y, float size, const ULBrush& brush) {
    float s = size <= 0 ? 1.0f : size;
    float h = s * 0.5f;
    drawRect(x - h, y - h, s, s, brush);
}

void Renderer::drawBorder(float x, float y, float w, float h, const Border& border) {
    if (border.isEmpty() || w <= 0 || h <= 0) return;
    const float l = static_cast<float>(std::max(0.0, border.left));
    const float t = static_cast<float>(std::max(0.0, border.top));
    const float r = static_cast<float>(std::max(0.0, border.right));
    const float b = static_cast<float>(std::max(0.0, border.bottom));
    if (l <= 0 && t <= 0 && r <= 0 && b <= 0) return;

    // 圆角半径 clamp 到矩形可容纳的范围
    const float radius = std::clamp(static_cast<float>(border.radius), 0.0f, 0.5f * std::min(w, h));

    const ULBrush brush(border.color);
    if (border.style == Border::Style::Solid) {
        // 实线：4 条直边矩形 + 每个角两条 45° 圆环扇区（按对角线切分，
        // 相邻两边线宽不同时也能正确拼合）
        if (t > 0) drawRect(x + radius, y, w - 2.0f * radius, t, brush);         // 上
        if (r > 0) drawRect(x + w - r, y + radius, r, h - 2.0f * radius, brush); // 右
        if (b > 0) drawRect(x + radius, y + h - b, w - 2.0f * radius, b, brush); // 下
        if (l > 0) drawRect(x, y + radius, l, h - 2.0f * radius, brush);         // 左

        if (radius > 0) {
            const float cxTL = x + radius, cyTL = y + radius;         // 左上角圆心
            const float cxTR = x + w - radius, cyTR = y + radius;     // 右上角圆心
            const float cxBR = x + w - radius, cyBR = y + h - radius; // 右下角圆心
            const float cxBL = x + radius, cyBL = y + h - radius;     // 左下角圆心
            // 角度约定：0° 朝右、90° 朝下（y 向下）
            // 左上角（上边 + 左边）
            if (t > 0) drawArc(cxTL, cyTL, radius - 0.5f * t, 225, 270, t, brush);
            if (l > 0) drawArc(cxTL, cyTL, radius - 0.5f * l, 180, 225, l, brush);
            // 右上角（上边 + 右边）
            if (t > 0) drawArc(cxTR, cyTR, radius - 0.5f * t, 270, 315, t, brush);
            if (r > 0) drawArc(cxTR, cyTR, radius - 0.5f * r, 315, 360, r, brush);
            // 右下角（右边 + 下边）
            if (r > 0) drawArc(cxBR, cyBR, radius - 0.5f * r, 0, 45, r, brush);
            if (b > 0) drawArc(cxBR, cyBR, radius - 0.5f * b, 45, 90, b, brush);
            // 左下角（下边 + 左边）
            if (b > 0) drawArc(cxBL, cyBL, radius - 0.5f * b, 90, 135, b, brush);
            if (l > 0) drawArc(cxBL, cyBL, radius - 0.5f * l, 135, 180, l, brush);
        }
        return;
    }

    // 虚线/点线：沿边框中线折线连续走相位，避免各边图案错位。
    // 中线 = 各边向内偏移半线宽；圆角处按两条 45° 弧拼合，非均匀线宽时两弧半径不同，
    // 在对角线处有微小错位，视觉上可忽略。
    struct DashSeg {
        float x0, y0, x1, y1, width;
    };
    std::vector<DashSeg> segs;
    auto pushLine = [&segs](float x0, float y0, float x1, float y1, float width) {
        if (width > 0) segs.push_back({x0, y0, x1, y1, width});
    };
    auto pushArcPiece = [&pushLine](float cx, float cy, float rad, float a0Deg, float a1Deg, float width) {
        if (rad <= 0 || width <= 0) return;
        const float arcLen = rad * (a1Deg - a0Deg) * kPi / 180.0f;
        const int n        = std::min(kMaxDashArcSegments, std::max(1, static_cast<int>(std::ceil(arcLen))));
        const float step   = (a1Deg - a0Deg) / static_cast<float>(n) * kPi / 180.0f;
        float a            = a0Deg * kPi / 180.0f;
        for (int i = 0; i < n; ++i) {
            const float a1 = a + step;
            pushLine(cx + rad * std::cos(a), cy + rad * std::sin(a), cx + rad * std::cos(a1), cy + rad * std::sin(a1),
                     width);
            a = a1;
        }
    };
    // 圆角半径足够时用两条 45° 弧拼角；否则（含直角）直接连接相邻两边的中线端点
    auto pushCorner = [&](float cx, float cy, float radA, float aA0, float aA1, float wA, float radB, float aB0,
                          float aB1, float wB, float jx0, float jy0, float jx1, float jy1) {
        if (radA > 0 && radB > 0) {
            pushArcPiece(cx, cy, radA, aA0, aA1, wA);
            pushArcPiece(cx, cy, radB, aB0, aB1, wB);
        } else {
            pushLine(jx0, jy0, jx1, jy1, 0.5f * (wA + wB));
        }
    };

    const float tcy = y + 0.5f * t, rcx = x + w - 0.5f * r; // 上边中线的 y、右边中线的 x
    const float bcy = y + h - 0.5f * b, lcx = x + 0.5f * l; // 下边中线的 y、左边中线的 x
    const float radT = radius - 0.5f * t, radR = radius - 0.5f * r;
    const float radB = radius - 0.5f * b, radL = radius - 0.5f * l;

    // 中线折线（顺时针）：上 → 右上角 → 右 → 右下角 → 下 → 左下角 → 左 → 左上角
    pushLine(x + radius, tcy, x + w - radius, tcy, t);                                                           // 上
    pushCorner(x + w - radius, y + radius, radT, 270, 315, t, radR, 315, 360, r, x + w - radius, tcy, rcx, y + radius);    // 右上角
    pushLine(rcx, y + radius, rcx, y + h - radius, r);                                                           // 右
    pushCorner(x + w - radius, y + h - radius, radR, 0, 45, r, radB, 45, 90, b, rcx, y + h - radius, x + w - radius, bcy); // 右下角
    pushLine(x + w - radius, bcy, x + radius, bcy, b);                                                           // 下
    pushCorner(x + radius, y + h - radius, radB, 90, 135, b, radL, 135, 180, l, x + radius, bcy, lcx, y + h - radius);     // 左下角
    pushLine(lcx, y + h - radius, lcx, y + radius, l);                                                           // 左
    pushCorner(x + radius, y + radius, radL, 180, 225, l, radT, 225, 270, t, lcx, y + radius, x + radius, tcy);            // 左上角

    // 连续相位走线：dashRem 为当前实/空段的剩余长度，跨段（跨边、跨角）连续
    const bool dotted = border.style == Border::Style::Dotted;
    bool inDash       = false; // 首轮翻转后从实段开始
    float dashRem     = 0.0f;
    for (const auto& s : segs) {
        const float dashLen = s.width * (dotted ? kDotLengthScale : kDashLengthScale);
        const float gapLen  = s.width * kGapLengthScale;
        const float dx      = s.x1 - s.x0;
        const float dy      = s.y1 - s.y0;
        const float segLen  = std::sqrt(dx * dx + dy * dy);
        if (segLen < 1e-4f) continue;

        float t = 0.0f;
        while (t < segLen) {
            if (dashRem <= 0) {
                inDash  = !inDash;
                dashRem = inDash ? dashLen : gapLen;
            }
            const float take = std::min(dashRem, segLen - t);
            if (inDash) {
                const float k0 = t / segLen;
                const float k1 = (t + take) / segLen;
                drawLine(s.x0 + dx * k0, s.y0 + dy * k0, s.x0 + dx * k1, s.y0 + dy * k1, s.width, brush);
            }
            t       += take;
            dashRem -= take;
        }
    }
}

void Renderer::applyBrush(const ULBrush& brush) {
    // 一批顶点只能共用一套渐变 uniform；画刷变化时先 flush 掉旧批次
    if (_vertexCount != 0 && !(_brush == brush)) { flush(); }
    _brush = brush;
}

void Renderer::uploadBrushUniforms(const ULBrush& brush) {
    const auto& gradient = brush.gradient();
    if (!gradient) {
        _program->setInt("uBrushType", 0);
        return;
    }

    const bool linear = brush.style() == BrushStyle::LinearGradient;
    _program->setInt("uBrushType", linear ? 1 : 2);
    _program->setInt("uSpread", static_cast<int>(gradient->spread()));

    const auto& stops = gradient->stops();
    const int count   = std::min(static_cast<int>(stops.size()), kMaxGradientStops);
    _program->setInt("uStopCount", count);

    std::vector<float> pos(kMaxGradientStops, 0.0f);
    std::vector<float> col(kMaxGradientStops * 4, 0.0f);
    for (int i = 0; i < count; ++i) {
        pos[i]         = stops[i].first;
        const RGBA& c  = stops[i].second;
        col[i * 4 + 0] = static_cast<float>(c.red()) / 255.0f;
        col[i * 4 + 1] = static_cast<float>(c.green()) / 255.0f;
        col[i * 4 + 2] = static_cast<float>(c.blue()) / 255.0f;
        col[i * 4 + 3] = static_cast<float>(c.alpha()) / 255.0f;
    }
    _program->setFloatArray("uStopPos", kMaxGradientStops, pos.data());
    _program->setVec4Array("uStopColor", kMaxGradientStops, col.data());

    if (linear) {
        const auto* lin = dynamic_cast<const LinearGradient*>(gradient.get());
        if (lin) {
            const float start[2] = {lin->x0(), lin->y0()};
            const float stop[2]  = {lin->x1(), lin->y1()};
            _program->setVec2fv("uGradStart", start);
            _program->setVec2fv("uGradStop", stop);
        }
    } else {
        const auto* rad = dynamic_cast<const RadiusGradient*>(gradient.get());
        if (rad) {
            const float center[2] = {rad->cx(), rad->cy()};
            _program->setVec2fv("uGradCenter", center);
            _program->setFloat("uGradRadius", rad->radius());
        }
    }
}

void Renderer::flush() {
    if (!_inited || _vertexCount == 0) return;

    _program->use();
    _program->setMatrix4fv("projection", &_projection[0][0]);
    _program->setInt("tex", 0);
    uploadBrushUniforms(_brush);
    _texture->drawTexture(GL_TEXTURE0);

    _vao->use();
    _vao->updateData(_buffer);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(_vertexCount));
    glBindVertexArray(0);

    _buffer.clear();
    _vertexCount = 0;
}

} // namespace ULGui::base
