//
// Created by cxn on 2026/10/18.
// 圆角矩形 SDF 实例化绘制器：一个 quad（6 顶点）一个实例，
// 形状由 fragment shader 的 SDF 决定（支持四角不同半径、四边不同描边宽度）。
//

#ifndef _ROUNDED_RECT_DRAWER_H
#define _ROUNDED_RECT_DRAWER_H

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "border.h"
#include "brush.h"
#include "program.h"
#include "vertex_array_obj.h"

namespace ULGui::base {

/**
 * 圆角矩形 SDF 实例化绘制器。
 * 实例布局（16 floats）：rect(x,y,w,h) + radii(lt,rt,rb,lb) + border(wL,wT,wR,wB) + color(rgba)。
 * 填充支持渐变画刷（复用 renderer.frag 的 uBrush* uniform 方案，采样世界坐标 vPos）；
 *
 * 实线描边（任一 border 宽 > 0）在 shader 里挖掉内盒形成环带，颜色走实例自带的 aColor。
 * 段/命令列表由 Renderer 持有，本类只负责实例数据的记录与绘制。
 */
class RoundedRectDrawer {
public:
    static constexpr uint32_t kMaxInstances      = 4096; // 单帧实例容量
    static constexpr uint32_t kFloatsPerInstance = 16;
    static constexpr uint32_t kQuadVertices      = 6;

    RoundedRectDrawer() = default;
    ~RoundedRectDrawer();
    RoundedRectDrawer(const RoundedRectDrawer&)            = delete;
    RoundedRectDrawer(RoundedRectDrawer&&)                 = delete;
    RoundedRectDrawer& operator=(const RoundedRectDrawer&) = delete;
    RoundedRectDrawer& operator=(RoundedRectDrawer&&)      = delete;

    // 编译 roundedRect.vert/.frag、创建 quad/实例 VAO；GL 上下文就绪后调用
    bool init();
    void shutdown();

    // —— 记录阶段：只追加实例数据，不做 GL 调用 ——
    // 填充（支持渐变画刷）；radii 会被 clamp 到 [0, 0.5*min(w,h)]
    void pushFill(float x, float y, float w, float h, const BorderRadius& radius, const ULBrush& brush);
    // 实线描边；任一线宽 <= 0 时跳过（负宽 clamp 为 0）；radii 同样 clamp
    void pushStroke(float x,
                    float y,
                    float w,
                    float h,
                    const BorderRadius& radius,
                    float wLeft,
                    float wTop,
                    float wRight,
                    float wBottom,
                    const RGBA& color);

    bool isFull() const { return _instanceCount >= kMaxInstances; }
    uint32_t instanceCount() const { return _instanceCount; }

    // glBufferSubData 一次性上传全部实例
    void updateInstanceBuffer();
    // 绘制实例区间 [first, first+count)：bind program、上传 projection/brush uniform、bind VAO、
    // glDrawArraysInstanced。GL 3.3 无 base-instance，内部重设 attrib 1..4 的指针偏移实现。
    void drawRange(uint32_t first, uint32_t count, const glm::mat4& projection, const ULBrush& brush);
    // 清空实例暂存（每帧 flush 结束后由 Renderer 调用）
    void clear();

private:
    bool createVAO();
    void appendInstance(const float* inst);

    std::unique_ptr<myUtil::Program> _program;
    std::unique_ptr<myUtil::VertexArrayObj> _vao;
    std::shared_ptr<myUtil::BufferF> _quadBuffer;     // 静态 6x2 单位 quad
    std::shared_ptr<myUtil::BufferF> _instanceBuffer; // 动态 kMaxInstances x 16
    std::vector<float> _instances;
    uint32_t _instanceCount {0};
    bool _inited {false};
};

// 把 ULBrush（纯色/线性/径向渐变）写成 uniform：Renderer 主 program 与 RoundedRectDrawer 共用。
// 原 Renderer::uploadBrushUniforms 平移而来，显式传 Program&；不碰 tex（rect program 没有）。
void uploadBrushUniforms(myUtil::Program& program, const ULBrush& brush);

} // namespace ULGui::base

#endif // _ROUNDED_RECT_DRAWER_H
