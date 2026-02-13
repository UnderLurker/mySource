#ifndef _SIZE_H
#define _SIZE_H

#include <cstdint>

#include "utils/macro.h"

namespace ULGui::canvas {
template<typename T>
class Size {
public:
    DECLARE_MOVE_COPY(Size)
    Size() = default;
    Size(const T& width, const T& height)
        : _width(width), _height(height) {}
    T& width() { return _width; }
    T& height() { return _height; }
    constexpr T width() const { return _width; }
    constexpr T height() const { return _height; }

    bool operator==(const Size& obj) { return width() == obj.width() && height() == obj.height(); }

private:
    T _width;
    T _height;
};

using SizeF = Size<float>;
using SizeI = Size<int32_t>;
} // namespace ULGui::canvas

#endif // !_SIZE_H
