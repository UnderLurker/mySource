//
// Created by XW on 2024/5/16.
//
#include "baseGraphics.h"

#include <cmath>
#include <vector>

namespace ULGui::base {

void Triangle::paintEvent(event::PaintEvent* event) {
    setLineMode(base::LINE_LOOP);
    lineTo({_pos[0], _pos[1], _pos[2]});
}

void Rectangle::setPosition(const RectPosition& pos) {
    top    = pos[0];
    left   = pos[1];
    bottom = pos[2];
    right  = pos[3];
}

void Rectangle::paintEvent(event::PaintEvent* event) {
    setLineMode(base::LINE_LOOP);
    lineTo({
        {left,  top   },
        {right, top   },
        {right, bottom},
        {left,  bottom}
    });
}

void Circle::paintEvent(event::PaintEvent* event) {
    setLineMode(base::LINE_STRIP);
    auto size = (size_t)std::round((endAngle - startAngle) / DEFAULT_CIRCLE_PRECISION);
    std::vector<Point> pointList(size);
    for (size_t i = 0; i < size; i++) {
        double temp  = (startAngle + i * DEFAULT_CIRCLE_PRECISION) * M_PI / 180.0f;
        pointList[i] = {center.x + radius * cos(temp), center.y - radius * sin(temp)};
    }
    lineTo(pointList);
}
} // namespace ULGui::base
