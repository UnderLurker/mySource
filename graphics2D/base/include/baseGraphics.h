//
// Created by XW on 2024/5/16.
//

#ifndef _BASE_GRAPHICS_H
#define _BASE_GRAPHICS_H

#include "abstractAbility.h"
#include "graphic2DType.h"

namespace graphics2D::base {

class Rectangle : public virtual AbstractAbility {
public:
    double _top{0};
    double _left{0};
    double _buttom{0};
    double _right{0};
};

}

#endif // _BASE_GRAPHICS_H
