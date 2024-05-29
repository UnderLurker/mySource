//
// Created by XW on 2024/5/16.
//
#include "baseGraphics.h"

#include <vector>

namespace ULGui::base {

void Triangle::paintEvent(event::PaintEvent* event) {
    setLineMode(base::LINE_LOOP);
    lineTo({_pos[0], _pos[1], _pos[2]});
}

}
