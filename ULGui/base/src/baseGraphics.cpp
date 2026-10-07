//
// Created by XW on 2024/5/16.
//
#include "baseGraphics.h"

#include <cmath>
#include <vector>

#include "include/logger_wrapper.h"

namespace ULGui::base {

void Triangle::paintEvent(event::PaintEvent*) {
    setLineMode(base::LINE_LOOP);
    lineTo({_pos[0], _pos[1], _pos[2]});
}

void Rectangle::setPosition(const RectPosition& pos) {
    left   = pos[0];
    top    = pos[1];
    right  = pos[2];
    bottom = pos[3];
}

void Rectangle::paintEvent(event::PaintEvent*) {
    drawRect(Point {left, top}, right - left, bottom - top);
}

void Circle::paintEvent(event::PaintEvent*) {
    if (std::abs(endAngle - startAngle) == 360) {
        circle(center, radius, fill);
    } else {
        arc(center, radius, startAngle, endAngle);
    }
}
} // namespace ULGui::base
