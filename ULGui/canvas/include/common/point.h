#ifndef _POINT_H
#define _POINT_H

#include <cstdint>

#include "utils/macro.h"

namespace ULGui::canvas {
template<typename T>
struct Point {
    T x;
    T y;
};

using PointF = Point<float>;
using PointI = Point<int32_t>;
} // namespace ULGui::canvas

#endif // !_POINT_H
