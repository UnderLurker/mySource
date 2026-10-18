//
// Created by cxn on 2026/10/18.
//
#include "roundedRectDrawer.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include "include/logger_wrapper.h"

namespace ULGui::base {

namespace {
constexpr int kMaxGradientStops  = 16; // shader 中颜色 stop 数组的上限
const std::string RECT_VERT_PATH = "base/shader/roundedRect.vert";
const std::string RECT_FRAG_PATH = "base/shader/roundedRect.frag";

void clampRadii(const BorderRadius& radius, float w, float h, float* out) {
    const float m = 0.5f * std::min(w, h);
    out[0]        = std::clamp(static_cast<float>(radius.leftTop), 0.0f, m);
    out[1]        = std::clamp(static_cast<float>(radius.rightTop), 0.0f, m);
    out[2]        = std::clamp(static_cast<float>(radius.rightBottom), 0.0f, m);
    out[3]        = std::clamp(static_cast<float>(radius.leftBottom), 0.0f, m);
}

RGBA fillColor(const ULBrush& brush) {
    const auto style = brush.style();
    if (style == BrushStyle::LinearGradient || style == BrushStyle::RadialGradient) return RGBA(255, 255, 255, 255);
    return brush.color();
}
} // namespace

RoundedRectDrawer::~RoundedRectDrawer() { shutdown(); }

bool RoundedRectDrawer::init() {
    if (_inited) return true;
    _program = std::make_unique<myUtil::Program>();
    if (!_program) {
        LOGE("[RoundedRectDrawer] create Program fail.");
        return false;
    }
    _program->setName("RoundedRectDrawer");
    std::unique_ptr<myUtil::Shader> vShader = std::make_unique<myUtil::VertexShader>(RECT_VERT_PATH);
    std::unique_ptr<myUtil::Shader> fShader = std::make_unique<myUtil::FragmentShader>(RECT_FRAG_PATH);
    if (!vShader || !fShader) return false;
    _program->push_back(std::move(vShader));
    _program->push_back(std::move(fShader));
    if (!_program->linkProgram()) {
        LOGE("[RoundedRectDrawer] linkProgram fail!!!");
        return false;
    }

    _quadBuffer     = std::make_shared<myUtil::BufferF>(myUtil::BufferType::VBO);
    _instanceBuffer = std::make_shared<myUtil::BufferF>(myUtil::BufferType::VBO);
    _vao            = std::make_unique<myUtil::VertexArrayObj>();
    if (!_quadBuffer || !_instanceBuffer || !_vao) {
        LOGE("[RoundedRectDrawer] create buffer/vao fail.");
        return false;
    }
    if (!createVAO()) return false;
    glBindVertexArray(0);
    _inited = true;
    return true;
}

void RoundedRectDrawer::shutdown() {
    _instances.clear();
    _instanceCount = 0;
    _quadBuffer.reset();
    _instanceBuffer.reset();
    _vao.reset();
    _program.reset();
    _inited = false;
}

bool RoundedRectDrawer::createVAO() {
    // 单位 quad：两个三角形，6 顶点；静态
    const std::vector<float> quad = {0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1};
    _quadBuffer->setData(quad, myUtil::BufferUsage::STATIC_DRAW);
    _instanceBuffer->setData(static_cast<int64_t>(kMaxInstances) * kFloatsPerInstance,
                             myUtil::BufferUsage::DYNAMIC_DRAW);
    _instances.reserve(static_cast<size_t>(kMaxInstances) * kFloatsPerInstance);

    // attrib 0: quad 顶点（divisor 0）；attrib 1..4: 实例数据（divisor 1）。
    _vao->setVertexAttribPointer(_quadBuffer, {.attrIndex = 0, .size = 2, .stride = 2, .offset = 0});
    _vao->setVertexAttribPointer(_instanceBuffer, {.attrIndex = 1, .size = 4, .stride = 16, .offset = 0, .divisor = 1});
    _vao->setVertexAttribPointer(_instanceBuffer, {.attrIndex = 2, .size = 4, .stride = 16, .offset = 4, .divisor = 1});
    _vao->setVertexAttribPointer(_instanceBuffer, {.attrIndex = 3, .size = 4, .stride = 16, .offset = 8, .divisor = 1});
    _vao->setVertexAttribPointer(_instanceBuffer,
                                 {.attrIndex = 4, .size = 4, .stride = 16, .offset = 12, .divisor = 1});
    return true;
}

void RoundedRectDrawer::appendInstance(const float* inst) {
    _instances.insert(_instances.end(), inst, inst + kFloatsPerInstance);
    ++_instanceCount;
}

void RoundedRectDrawer::pushFill(float x, float y, float w, float h, const BorderRadius& radius, const ULBrush& brush) {
    if (!_inited || _instanceCount >= kMaxInstances || w <= 0.0f || h <= 0.0f) return;
    float r[4];
    clampRadii(radius, w, h, r);
    const RGBA c                         = fillColor(brush);
    const float inst[kFloatsPerInstance] = {
        x,
        y,
        w,
        h,
        r[0],
        r[1],
        r[2],
        r[3],
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        static_cast<float>(c.red()) / 255.0f,
        static_cast<float>(c.green()) / 255.0f,
        static_cast<float>(c.blue()) / 255.0f,
        static_cast<float>(c.alpha()) / 255.0f,
    };
    appendInstance(inst);
}

void RoundedRectDrawer::pushStroke(float x,
                                   float y,
                                   float w,
                                   float h,
                                   const BorderRadius& radius,
                                   float wLeft,
                                   float wTop,
                                   float wRight,
                                   float wBottom,
                                   const RGBA& color) {
    if (!_inited || _instanceCount >= kMaxInstances || w <= 0.0f || h <= 0.0f) return;
    wLeft   = std::max(0.0f, wLeft);
    wTop    = std::max(0.0f, wTop);
    wRight  = std::max(0.0f, wRight);
    wBottom = std::max(0.0f, wBottom);
    if (wLeft <= 0.0f && wTop <= 0.0f && wRight <= 0.0f && wBottom <= 0.0f) return;
    float r[4];
    clampRadii(radius, w, h, r);
    const float inst[kFloatsPerInstance] = {
        x,
        y,
        w,
        h,
        r[0],
        r[1],
        r[2],
        r[3],
        wLeft,
        wTop,
        wRight,
        wBottom,
        static_cast<float>(color.red()) / 255.0f,
        static_cast<float>(color.green()) / 255.0f,
        static_cast<float>(color.blue()) / 255.0f,
        static_cast<float>(color.alpha()) / 255.0f,
    };
    appendInstance(inst);
}

void RoundedRectDrawer::updateInstanceBuffer() {
    if (!_inited || _instanceCount == 0) return;
    _instanceBuffer->update(_instances);
}

void RoundedRectDrawer::drawRange(uint32_t, uint32_t count, const glm::mat4& projection, const ULBrush& brush) {
    if (!_inited || count == 0) return;
    _program->bind();
    _program->setMatrix4fv("projection", glm::value_ptr(projection));
    uploadBrushUniforms(*_program, brush);
    _vao->bind();
    // GL 3.3 core 没有 glDrawArraysInstancedBaseInstance：实例数据固定从缓冲第 0 个元素读起。
    // 通过重设 attrib 1..4 的指针偏移，让本次 draw 的实例 i 读到 first+i。
    // divisor 已随 VAO 状态保存，glVertexAttribPointer 不会改动 divisor。
    // _instanceBuffer->bind();
    // constexpr GLsizei stride = static_cast<GLsizei>(kFloatsPerInstance * sizeof(float));
    // const GLintptr base      = static_cast<GLintptr>(first) * stride;
    // for (uint32_t a = 1; a <= 4; ++a) {
    //     glVertexAttribPointer(static_cast<GLuint>(a), 4, GL_FLOAT, GL_FALSE, stride,
    //                           reinterpret_cast<const void*>(
    //                               static_cast<uintptr_t>(base + (a - 1) * 4 * sizeof(float))));
    // }
    glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(kQuadVertices), static_cast<GLsizei>(count));
}

void RoundedRectDrawer::clear() {
    _instances.clear();
    _instanceCount = 0;
}

void uploadBrushUniforms(myUtil::Program& program, const ULBrush& brush) {
    const auto& gradient = brush.gradient();
    if (!gradient) {
        program.setInt("uBrushType", 0);
        return;
    }

    const bool linear = brush.style() == BrushStyle::LinearGradient;
    program.setInt("uBrushType", linear ? 1 : 2);
    program.setInt("uSpread", static_cast<int>(gradient->spread()));

    const auto& stops = gradient->stops();
    const int count   = std::min(static_cast<int>(stops.size()), kMaxGradientStops);
    program.setInt("uStopCount", count);

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
    program.setFloatArray("uStopPos", kMaxGradientStops, pos.data());
    program.setVec4Array("uStopColor", kMaxGradientStops, col.data());

    if (linear) {
        const auto* lin = dynamic_cast<const LinearGradient*>(gradient.get());
        if (lin) {
            const float start[2] = {lin->x0(), lin->y0()};
            const float stop[2]  = {lin->x1(), lin->y1()};
            program.setVec2fv("uGradStart", start);
            program.setVec2fv("uGradStop", stop);
        }
    } else {
        const auto* rad = dynamic_cast<const RadiusGradient*>(gradient.get());
        if (rad) {
            const float center[2] = {rad->cx(), rad->cy()};
            program.setVec2fv("uGradCenter", center);
            program.setFloat("uGradRadius", rad->radius());
        }
    }
}

} // namespace ULGui::base
