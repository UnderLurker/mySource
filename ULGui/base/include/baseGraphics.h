//
// Created by XW on 2024/5/16.
//

#ifndef _BASE_GRAPHICS_H
#define _BASE_GRAPHICS_H

#include "abstractAbility.h"
#include "ULGuiType.h"

namespace ULGui::base {

class Rectangle : public virtual AbstractAbility {
public:
    float& top() { return _data[0]; }
    float& left() { return _data[1]; }
    float& bottom() { return _data[2]; }
    float& right() { return _data[3]; }
    explicit operator GVec4f() {
        return _data;
    }

private:
    // {top, left, bottom, right}
    GVec4f _data;
};

} // namespace graphics2D::base

#endif // _BASE_GRAPHICS_H
