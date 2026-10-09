//
// Created by cxn on 2026/10/8.
//

#ifndef _BORDER_H
#define _BORDER_H

#include <cstdint>
#include <string>

#include "ULGuiColor.h"

namespace ULGui::base {

/**
 * 边框描述：四边线宽 + 颜色 + 线型 + 圆角半径。
 * 只描述样式，绘制位置与尺寸由 Renderer::drawBorder
 * 传入的矩形决定。
 */
struct Border {
    // 线型
    enum class Style : uint8_t {
        Solid,  // 实线
        Dashed, // 虚线
        Dotted, // 点线
    };

    double left {0.0};
    double top {0.0};
    double right {0.0};
    double bottom {0.0};
    RGBA color {RGBA::BLACK};
    Style style {Style::Solid};
    double radius {0.0}; // 圆角半径，0 表示直角

    Border() = default;
    explicit Border(double uniform)
        : left(uniform), top(uniform), right(uniform), bottom(uniform) {}
    Border(double l, double t, double r, double b)
        : left(l), top(t), right(r), bottom(b) {}

    bool isEmpty() const { return left <= 0 && top <= 0 && right <= 0 && bottom <= 0; }
    bool isUniform() const { return left == top && top == right && right == bottom; }

    bool operator==(const Border& obj) const {
        return left == obj.left && top == obj.top && right == obj.right && bottom == obj.bottom &&
               static_cast<uint32_t>(color) == static_cast<uint32_t>(obj.color) && style == obj.style &&
               radius == obj.radius;
    }
    bool operator!=(const Border& obj) const { return !(*this == obj); }

    std::string toString() const {
        const char* styleStr = style == Style::Solid ? "Solid" : (style == Style::Dashed ? "Dashed" : "Dotted");
        return "{left: " + std::to_string(left) + ", top: " + std::to_string(top) +
               ", right: " + std::to_string(right) + ", bottom: " + std::to_string(bottom) +
               ", radius: " + std::to_string(radius) + ", style: " + styleStr + "}";
    }
};

} // namespace ULGui::base

#endif // !_BORDER_H
