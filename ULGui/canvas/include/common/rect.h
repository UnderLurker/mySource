#ifndef _COMMON_H
#define _COMMON_H

#include <cassert>
#include <sstream>
#include <string>

#include "point.h"
#include "size.h"
#include "utils/hasToString.h"
#include "utils/macro.h"

namespace ULGui::canvas {

template<typename T>
class Rect {
public:
    DECLARE_MOVE_COPY(Rect)
    Rect() = default;
    Rect(const Point<T>& topLeft, const Size<T>& size)
        : _left(topLeft.x), _top(topLeft.y), _right(topLeft.x + size.width()), _bottom(topLeft.y + size.height()) {}
    Rect(const T& left, const T& top, const T& right, const T& bottom)
        : _left(left), _right(right), _top(top), _bottom(bottom) {}
    T& left() { return _left; }
    T& right() { return _right; }
    T& top() { return _top; }
    T& bottom() { return _bottom; }
    constexpr T left() const { return _left; }
    constexpr T right() const { return _right; }
    constexpr T top() const { return _top; }
    constexpr T bottom() const { return _bottom; }
    constexpr T width() const { return _right - _left; }
    constexpr T height() const { return _bottom - _top; }
    constexpr T centerX() const { return (_left + _right) / 2; }
    constexpr T centerY() const { return (_top + _bottom) / 2; }
    constexpr Point<T> center() const { return Point<T> {centerX(), centerY()}; }
    constexpr Point<T> topLeft() const { return Point<T> {_left, _top}; }
    constexpr Size<T> size() const { return Size<T>(width(), height()); }

    std::string toString() const {
        std::stringstream ss;
        ss << "(" << convertString(_left) << ", ";
        ss << convertString(_top) << ") [";
        ss << convertString(_right) << " x ";
        ss << convertString(_bottom) << "]";
        return std::move(ss.str());
    }
    bool operator==(const Rect& obj) const {
        return left() == obj.left() && right() == obj.right() && top() == obj.top() && bottom() == obj.bottom();
    }

protected:
    template<typename U>
    static std::string convertString(const U& obj) {
        if constexpr (std::is_arithmetic_v<U>) {
            return std::move(std::to_string(obj));
        } else if constexpr (util::hasToString_v<U>) {
            return obj.toString();
        } else {
            // U is not toString or to_string.
            return "NA";
        }
    }

private:
    T _left;
    T _right;
    T _top;
    T _bottom;
};

using RectF = Rect<float>;
using RectI = Rect<int32_t>;
} // namespace ULGui::canvas

#endif // !_COMMON_H
