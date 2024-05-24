//
// Created by XW on 2024/5/23.
//

#ifndef _COORD_H
#define _COORD_H

#include <cassert>

#include "graphic2DType.h"

namespace graphics2D::base {

struct Point {
    double x {.0f};
    double y {.0f};
    double z {.0f};

    Point(double X, double Y, double Z = 0)
        : x(X), y(Y), z(Z) {}

    Point operator+(const Point& point) const { return {x + point.x, y + point.y, z + point.z}; }
    Point operator-(const Point& point) const { return {x - point.x, y - point.y, z - point.z}; }
    Point operator*(const double& val) const { return {x * val, y * val, z * val}; }
    Point operator/(const double& val) const {
        assert(val != 0);
        return {x / val, y / val, z / val};
    }
};

class Coord {
public:
    Coord() = default;
    explicit Coord(const GUint32& width, const GUint32& height, double x = 0, double y = 0, double z = 0)
        : _width(width), _height(height), _pos({x, y, z}) {}

    const Point& position() const { return _pos; }
    double X() const { return _pos.x; }
    double Y() const { return _pos.y; }
    double Z() const { return _pos.z; }
    GUint32 width() const { return _width; }
    GUint32 height() const { return _height; }

    void setPosition(const Point& pos) { _pos = pos; }
    void setWidth(const GUint32& width) { _width = width; }
    void setHeight(const GUint32& height) { _height = height; }

    /**
     * ��ȡ���ӿ��е�����
     * @return �����ӿ����귶Χ{[-1, 1], [-1, 1], 0}
     */
    Point toViewPort() const { return {toViewPort(_pos.x, _width), -1 * toViewPort(_pos.y, _height)}; }
    /**
     * ��ȡ���ӿ��е�����
     * @param input ��������
     * @return �����ӿ����귶Χ{[-1, 1], [-1, 1]}

     */
    Point toViewPort(const Point& input) { return {toViewPort(input.x, _width), -1 * toViewPort(input.y, _height)}; }
    static double toViewPort(const double& val, const double& base) { return val * 2 / base - 1; }

    Coord operator+(const Coord& coord) const;
    Coord operator-(const Coord& coord) const;
    Coord operator*(const double& val) const;
    Coord operator/(const double& val) const;

private:
    Point _pos {0, 0, 0};
    GUint32 _width {0};
    GUint32 _height {0};
};

} // namespace graphics2D

#endif // _COORD_H
