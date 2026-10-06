//
// Created by XW on 2024/5/23.
//
#include "coord.h"

namespace ULGui::base {
Point Coord::toViewPort() const { return {toViewPort(_pos.x, _width), -1 * toViewPort(_pos.y, _height)}; }

Point Coord::toViewPort(const Point& input) const {
    return {toViewPort(input.x, _width), -1 * toViewPort(input.y, _height)};
}
double Coord::toViewPort(const double& val, const double& base) {
    ULGUI_ASSERT(base != 0);
    return val * 2 / base - 1;
}

Coord Coord::operator+(const Coord& coord) const {
    Coord result(*this);
    result.setPosition(_pos + coord._pos);
    return result;
}

Coord Coord::operator-(const Coord& coord) const {
    Coord result(*this);
    result.setPosition(_pos - coord._pos);
    return result;
}

Coord Coord::operator*(const double& val) const {
    Coord result(*this);
    result.setPosition(_pos * val);
    return result;
}

Coord Coord::operator/(const double& val) const {
    Coord result(*this);
    result.setPosition(_pos / val);
    return result;
}
} // namespace ULGui::base
