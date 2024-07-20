//
// Created by 常笑男 on 2024/7/20.
//

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <string>

namespace ULGui::base {

template<typename T>
class Offset {
public:
    Offset() = default;
    constexpr explicit Offset(T x, T y)
        : _x(x), _y(y) {}

    T x() const { return _x; }
    T y() const { return _y; }
    void setX(T x) { _x = x; }
    void setY(T y) { _y = y; }

    constexpr Offset<T> operator+(const Offset<T>& obj) const { return Offset<T> {_x + obj._x, _y + obj._y}; }
    Offset<T>& operator+=(const Offset<T>& obj) {
        _x += obj._x;
        _y += obj._y;
        return *this;
    }
    constexpr Offset<T> operator-(const Offset<T>& obj) const { return Offset<T> {_x - obj._x, _y - obj._y}; }
    Offset<T>& operator-=(const Offset<T>& obj) {
        _x -= obj._x;
        _y -= obj._y;
        return *this;
    }
    constexpr bool operator==(const Offset<T>& obj) const { return _x == obj._x && _y == obj._y; }

    constexpr bool operator!=(const Offset<T>& obj) const { return _x != obj._x || _y != obj._y; }

    std::string toString() const { return "[" + _x.toString() + "," + _x.toString() + "]"; }

private:
    T _x;
    T _y;
};

template<typename T>
class Size {};

template<typename T>
class Rect {};

using OffsetF = Offset<float>;

class Geometry {
public:
private:
};

} // namespace ULGui::base

#endif // _GEOMETRY_H
