#ifndef _COMMON_H
#define _COMMON_H

#include <cassert>
#include <sstream>
#include <string>

#include "utils/hasToString.h"

namespace ULGui::canvas {

template<typename T>
class Rect {
public:
    constexpr T left() const { return _left; }
    constexpr T right() const { return _right; }
    constexpr T top() const { return _top; }
    constexpr T bottom() const { return _bottom; }
    constexpr T width() const { return _right - _left; }
    constexpr T height() const { return _bottom - _top; }
    constexpr T centerX() const { return (_left + _right) / 2; }
    constexpr T centerY() const { return (_top + _bottom) / 2; }

    std::string toString() const {
        std::stringstream ss;
        ss << "(" << convertString(_left) << ", ";
        ss << convertString(_right) << ") [";
        ss << convertString(_top) << " x ";
        ss << convertString(_bottom) << "]";
        return std::move(ss.str());
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

} // namespace ULGui::canvas

#endif // !_COMMON_H
