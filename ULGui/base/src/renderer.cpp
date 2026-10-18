//
// Created by XW on 2026/10/06.
//
#include "renderer.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <include/logger_wrapper.h>

#include "ULGuiColor.h"
#include "brush.h"
#include "program.h"

namespace ULGui::base {

namespace {
constexpr float kPi             = 3.14159265358979323846f;

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

    // 主批次绘制器（program + 1x1 白纹理 + 动态 VBO/VAO）
    _triangleBatch = std::make_unique<TriangleBatchDrawer>();
    if (!_triangleBatch) {
        LOGE("[Renderer] create TriangleBatchDrawer fail.");
        return false;
    }
    if (!_triangleBatch->init()) {
        LOGE("[Renderer] TriangleBatchDrawer init fail!!!");
        return false;
    }

    // 圆角矩形 SDF 绘制器（独立 program + 实例化 VAO）
    _roundedRectDrawer = std::make_unique<RoundedRectDrawer>();
    if (!_roundedRectDrawer) {
        LOGE("[Renderer] create RoundedRectDrawer fail.");
        return false;
    }
    if (!_roundedRectDrawer->init()) {
        LOGE("[Renderer] RoundedRectDrawer init fail!!!");
        return false;
    }

    glBindVertexArray(0);

    // 2D 不需要深度测试；开启混合支持半透明
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 抗锯齿：多重采样（配合窗口的 GLFW_SAMPLES hint）
    glEnable(GL_MULTISAMPLE);

    setViewport(viewportWidth, viewportHeight);

    _inited = true;
    return true;
}

void Renderer::shutdown() {
    if (!_inited) return;
    if (_triangleBatch) _triangleBatch->shutdown();
    _triangleBatch.reset();
    if (_roundedRectDrawer) _roundedRectDrawer->shutdown();
    _roundedRectDrawer.reset();
    _cmds.clear();
    _mainSegStart = 0;
    _rectSegStart = 0;
    _inited       = false;
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
    // 主图元接在圆角矩形之后：先关闭矩形段，保证命令列表按提交顺序回放
    if (_roundedRectDrawer->instanceCount() != _rectSegStart) {
        _cmds.push_back({DrawCmd::Kind::RoundedRect, _rectSegStart,
                         _roundedRectDrawer->instanceCount() - _rectSegStart, _rectBrush});
        _rectSegStart = _roundedRectDrawer->instanceCount();
    }
    _triangleBatch->pushVertex(x, y, c, u, v);
}

void Renderer::pushTriangle(float x0, float y0, float x1, float y1, float x2, float y2, const ULBrush& brush) {
    // 防溢出：顶点放不下时先 flush 掉已记录的批次（不在 pushVertex 里做，避免把三角形拆到两次 draw）
    if (_triangleBatch->vertexCount() + 3 > TriangleBatchDrawer::kMaxVertices) flush();
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

void Renderer::drawRoundedRectFill(float x,
                                   float y,
                                   float w,
                                   float h,
                                   const BorderRadius& radius,
                                   const ULBrush& brush) {
    if (w <= 0 || h <= 0) return;
    if (_roundedRectDrawer->isFull()) flush();
    closeMainSegment();
    // 描边实例不关心画刷，只有填充触发画刷分段
    if (_roundedRectDrawer->instanceCount() != _rectSegStart && !(_rectBrush == brush)) {
        _cmds.push_back({DrawCmd::Kind::RoundedRect, _rectSegStart,
                         _roundedRectDrawer->instanceCount() - _rectSegStart, _rectBrush});
        _rectSegStart = _roundedRectDrawer->instanceCount();
    }
    _rectBrush = brush;
    _roundedRectDrawer->pushFill(x, y, w, h, radius, brush);
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

void Renderer::drawSolidBorder(float x, float y, float w, float h, const Border& border) {
    if (w <= 0 || h <= 0) return;
    if (_roundedRectDrawer->isFull()) flush();
    closeMainSegment();
    // 描边实例自带颜色、忽略渐变 uniform → 加入当前矩形段即可，不触发画刷切换。
    // pushStroke 内部会对线宽 clamp 并在全 0 时跳过。
    _roundedRectDrawer->pushStroke(x, y, w, h, border.radius, static_cast<float>(border.left),
                                   static_cast<float>(border.top), static_cast<float>(border.right),
                                   static_cast<float>(border.bottom), border.color);
}

void Renderer::drawBorder(float x, float y, float w, float h, const Border& border) {
    if (border.isEmpty() || w <= 0 || h <= 0) return;
    if (border.style == Border::Style::Solid) {
        drawSolidBorder(x, y, w, h, border);
        return;
    }
    const float l = static_cast<float>(std::max(0.0, border.left));
    const float t = static_cast<float>(std::max(0.0, border.top));
    const float r = static_cast<float>(std::max(0.0, border.right));
    const float b = static_cast<float>(std::max(0.0, border.bottom));
    if (l <= 0 && t <= 0 && r <= 0 && b <= 0) return;

    // 圆角半径 clamp 到矩形可容纳的范围
    const float radiuslt = std::clamp(static_cast<float>(border.radius.leftTop), 0.0f, 0.5f * std::min(w, h));
    const float radiusrt = std::clamp(static_cast<float>(border.radius.rightTop), 0.0f, 0.5f * std::min(w, h));
    const float radiusrb = std::clamp(static_cast<float>(border.radius.rightBottom), 0.0f, 0.5f * std::min(w, h));
    const float radiuslb = std::clamp(static_cast<float>(border.radius.leftBottom), 0.0f, 0.5f * std::min(w, h));

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
    // 每条边两端角的圆角半径可能不同：弧半径必须按「本角半径 − 半线宽」逐角计算，
    // 否则弧在 0°/90°/180°/270° 处的端点与直边中线端点错位
    const float radLT_t = radiuslt - 0.5f * t, radLT_l = radiuslt - 0.5f * l; // 左上角：上边/左边弧
    const float radRT_t = radiusrt - 0.5f * t, radRT_r = radiusrt - 0.5f * r; // 右上角：上边/右边弧
    const float radRB_r = radiusrb - 0.5f * r, radRB_b = radiusrb - 0.5f * b; // 右下角：右边/下边弧
    const float radLB_b = radiuslb - 0.5f * b, radLB_l = radiuslb - 0.5f * l; // 左下角：下边/左边弧

    // 中线折线（顺时针）：上 → 右上角 → 右 → 右下角 → 下 → 左下角 → 左 → 左上角
    pushLine(x + radiuslt, tcy, x + w - radiusrt, tcy, t); // 上
    pushCorner(x + w - radiusrt, y + radiusrt, radRT_t, 270, 315, t, radRT_r, 315, 360, r, x + w - radiusrt, tcy, rcx,
               y + radiusrt);                              // 右上角
    pushLine(rcx, y + radiusrt, rcx, y + h - radiusrb, r); // 右
    pushCorner(x + w - radiusrb, y + h - radiusrb, radRB_r, 0, 45, r, radRB_b, 45, 90, b, rcx, y + h - radiusrb,
               x + w - radiusrb, bcy);                     // 右下角
    pushLine(x + w - radiusrb, bcy, x + radiuslb, bcy, b); // 下
    pushCorner(x + radiuslb, y + h - radiuslb, radLB_b, 90, 135, b, radLB_l, 135, 180, l, x + radiuslb, bcy, lcx,
               y + h - radiuslb);                          // 左下角
    pushLine(lcx, y + h - radiuslb, lcx, y + radiuslt, l); // 左
    pushCorner(x + radiuslt, y + radiuslt, radLT_l, 180, 225, l, radLT_t, 225, 270, t, lcx, y + radiuslt, x + radiuslt,
               tcy); // 左上角

    // 连续相位走线：dashRem 为当前实/空段的剩余长度，跨段（跨边、跨角）连续
    const bool dotted = border.style == Border::Style::Dotted;
    bool inDash       = false; // 首轮翻转后从实段开始
    float dashRem     = 0.0f;
    const ULBrush brush(border.color);
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
    // 一批顶点只能共用一套渐变 uniform；画刷变化时把旧批次收成一条命令，不再中途 flush
    if (_triangleBatch->vertexCount() != _mainSegStart && !(_brush == brush)) {
        _cmds.push_back({DrawCmd::Kind::Main, _mainSegStart,
                         _triangleBatch->vertexCount() - _mainSegStart, _brush});
        _mainSegStart = _triangleBatch->vertexCount();
    }
    _brush = brush;
}

void Renderer::closeMainSegment() {
    if (_triangleBatch->vertexCount() == _mainSegStart) return;
    _cmds.push_back({DrawCmd::Kind::Main, _mainSegStart,
                     _triangleBatch->vertexCount() - _mainSegStart, _brush});
    _mainSegStart = _triangleBatch->vertexCount();
}

void Renderer::flush() {
    if (!_inited) return;

    // 1. 关闭未关闭的段（跨类关段后最多只有一类有未关数据，双关无害）
    closeMainSegment();
    if (_roundedRectDrawer->instanceCount() > _rectSegStart) {
        _cmds.push_back({DrawCmd::Kind::RoundedRect, _rectSegStart,
                         _roundedRectDrawer->instanceCount() - _rectSegStart, _rectBrush});
        _rectSegStart = _roundedRectDrawer->instanceCount();
    }
    if (_cmds.empty()) return;

    // 2. 两块 VBO 各上传一次
    _triangleBatch->updateBuffer();
    _roundedRectDrawer->updateInstanceBuffer();

    // 3. 按提交顺序回放命令列表
    for (const auto& cmd : _cmds) {
        if (cmd.kind == DrawCmd::Kind::Main) {
            _triangleBatch->drawRange(cmd.first, cmd.count, _projection, cmd.brush);
        } else {
            _roundedRectDrawer->drawRange(cmd.first, cmd.count, _projection, cmd.brush);
        }
    }
    glBindVertexArray(0);

    // 4. 清空暂存与命令列表
    _mainSegStart = 0;
    _cmds.clear();
    _triangleBatch->clear();
    _roundedRectDrawer->clear();
    _rectSegStart = 0;
}
} // namespace ULGui::base
