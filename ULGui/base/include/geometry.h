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

    std::string toString() const { return "[ " + std::to_string(_x) + ", " + std::to_string(_y) + " ]"; }

private:
    T _x;
    T _y;
};

template<typename T>
class Size {
public:
    Size() = default;
    constexpr explicit Size(T width, T height)
        : _width(width), _height(height) {}

    T width() const { return _width; }
    T height() const { return _height; }
    void setWidth(T width) { _width = width; }
    void setHeight(T height) { _height = height; }

    constexpr Size<T> operator+(const Size<T>& obj) const { return Size {_width + obj._width, _height + obj._height}; }
    Size<T>& operator+=(const Size<T>& obj) {
        _width  += obj._width;
        _height += obj._height;
        return *this;
    }

    constexpr Size<T> operator-(const Size<T>& obj) const { return Size {_width - obj._width, _height - obj._height}; }
    Size<T>& operator-=(const Size<T>& obj) {
        _width  -= obj._width;
        _height -= obj._height;
        return *this;
    }
    constexpr bool operator==(const Offset<T>& obj) const { return _width == obj._width && _height == obj._height; }
    constexpr bool operator!=(const Offset<T>& obj) const { return _width != obj._width || _height != obj._height; }

    std::string toString() const { return "[ " + std::to_string(_width) + " x " + std::to_string(_height) + " ]"; }

private:
    T _width;
    T _height;
};


template<typename T>
class Rect {
public:
    Rect() = default;
    explicit Rect(const Offset<T>& offset, const Size<T>& size)
        : _offset(offset), _size(size) {}

    Offset<T> offset() const { return _offset; }
    Size<T> size() const { return _size; }
    void setOffset(const Offset<T>& offset) { _offset = offset; }
    void setSize(const Size<T>& size) { _size = size; }

    std::string toString() const { return _offset.toString() + _size.toString(); }

private:
    Offset<T> _offset;
    Size<T> _size;
};

using OffsetF = Offset<float>;
using SizeF   = Size<float>;
using RectF   = Rect<float>;
using PointF  = Offset<float>;

template<typename T>
class Circle {
public:
    Circle() = default;
    explicit Circle(const Offset<T>& center, T radius) : _center(center), _radius(radius){}

    Offset<T> center() const { return _center; }
    T radius() const { return _radius; }
    void setCenter(const Offset<T>& center) { _center = center; }
    void setRadius(T radius) { _radius = radius; }

    std::string toString() const { return "[ " + _center.toString() + ", " + std::to_string(_radius) + " ]"; }

private:
    Offset<T> _center;
    T _radius;
};


class Geometry {
public:
private:
};

} // namespace ULGui::base

#endif // _GEOMETRY_H
