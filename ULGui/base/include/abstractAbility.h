//
// Created by XW on 2024/5/23.
//

#ifndef _ABSTRACTABILITY_H
#define _ABSTRACTABILITY_H

#include <vector>

#include "coord.h"
#include "ULGuiType.h"

namespace ULGui::base {

// 线的类型 直线or点划线
enum LineType {
    NORMORAL,
    Stipple
};

// 控制是否点之间相连
enum LineMode {
    LINE       = 0x0001,
    LINE_LOOP  = 0x0002,
    LINE_STRIP = 0x0003
};

// 控制点划线样式
struct StippleStyle {
    GInt32 factor {DEFAULT_STIPPLE_FACTOR};
    GUint16 pattern {DEFAULT_STIPPLE_PATTERN};
};

// 线的样式
struct LineStyle {
    LineMode mode {LINE};
    LineType type {NORMORAL};
    float width {DEFAULT_PEN_WIDTH};
    GraphicRGBA color {DEFAULT_PEN_COLOR};
    StippleStyle stippleStyle;
};

class AbstractAbility {
public:
    AbstractAbility() = default;
    LineType lineType() const { return _style.type; }
    float penWidth() const { return _style.width; }
    GraphicRGBA penColor() const { return _style.color; }
    StippleStyle stippleStyle() const { return _style.stippleStyle; }
    LineMode lineMode() const { return _style.mode; }
    LineStyle lineStyle() const { return _style; }

    void setLineType(LineType type) { _style.type = type; }
    void setPenWidth(float width) { assert(width >= 0), _style.width = width; }
    void setPenColor(const GraphicRGBA& color) { _style.color = color; }
    void setStippleStyle(const StippleStyle& style) { _style.stippleStyle = style; }
    void setLineMode(const LineMode& mode) { _style.mode = mode; }
    void setLineStyle(const LineStyle& style) { _style = style; }
    void setCoordSize(const GInt32& width, const GInt32& height);

    void lineTo(const Coord& start, const Coord& end);
    void lineTo(const std::vector<Point>& pointList);
    void point(const Point& position);
    void point(const Coord& position);

protected:
    void updateStyle();

public:
    static GUint32 _count;

private:
    Coord _coord;
    GUint32 _id {0};
    LineStyle _style;
};

} // namespace ULGui::base

#endif // _ABSTRACTABILITY_H
