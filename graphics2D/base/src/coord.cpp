//
// Created by XW on 2024/5/23.
//
#include "coord.h"

namespace graphics2D::base {

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
} // namespace graphics2D
