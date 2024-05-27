//
// Created by XW on 2024/5/23.
//

#ifndef _ABSTRACTABILITY_H
#define _ABSTRACTABILITY_H

#include "coord.h"

namespace ULGui::base {

class AbstractAbility {
public:
    AbstractAbility() = default;
    float PenWidth() const { return _penWidth; }
    const GraphicRGBA& PenColor() const { return _penColor; }

    void setPenWidth(float width) {
        assert(width >= 0);
        _penWidth = width;
    }
    void setPenColor(const GraphicRGBA& color) { _penColor = color; }
    void setCoordSize(const GUint32& width, const GUint32& height);

    void lineTo(const Point& start, const Point& end);
    void lineTo(const Coord& start, const Coord& end);
    void point(const Point& position);
    void point(const Coord& position);

public:
    static GUint32 _count;

private:
    Coord _coord;
    GUint32 _id {0};
    float _penWidth {DEFAULT_PEN_WIDTH};
    GraphicRGBA _penColor {DEFAULT_PEN_COLOR};
};

} // namespace graphics2D::base

#endif // _ABSTRACTABILITY_H
