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
    if (abs(endAngle - startAngle) == 360) {
        circle(center, radius, fill);
    } else {
        arc(center, radius, startAngle, endAngle);
    }
}
} // namespace ULGui::base
