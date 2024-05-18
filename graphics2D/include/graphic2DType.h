//
// Created by XW on 2024/5/16.
//

#ifndef _GRAPHIC2DTYPE_H
#define _GRAPHIC2DTYPE_H

#include <cstdint>

#include "graphic2DMacro.h"

namespace graphics2D {

__GRAPHIC_2D_DECLARE typedef signed char GInt8;
__GRAPHIC_2D_DECLARE typedef unsigned char GUint8;
__GRAPHIC_2D_DECLARE typedef short GInt16;
__GRAPHIC_2D_DECLARE typedef unsigned short GUint16;
__GRAPHIC_2D_DECLARE typedef int GInt32;
__GRAPHIC_2D_DECLARE typedef unsigned GUint32;
__GRAPHIC_2D_DECLARE typedef long long GInt64;
__GRAPHIC_2D_DECLARE typedef unsigned long long GUint64;

__GRAPHIC_DECLARE struct GraphicRGBA {
    float red {255.0f};
    float green {255.0f};
    float blue {255.0f};
    float alpha {255.0f};
    explicit GraphicRGBA(float r, float g, float b, float a = 255.0f)
        : red(r), green(g), blue(b), alpha(a) {}
};
} // namespace graphics2D

#endif // _GRAPHIC2DTYPE_H
