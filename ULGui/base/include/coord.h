//
// Created by XW on 2024/5/23.
//

#ifndef _G_COORD_H
#define _G_COORD_H

#include <string>

#include "ULGuiMacro.h"

namespace ULGui::base {

struct Point {
    double x {.0f};
    double y {.0f};
    double z {.0f};

    Point()                 = default;
    Point(const Point& obj) = default;
    Point(double X, double Y, double Z = 0)
        : x(X), y(Y), z(Z) {}
    ~Point() = default;

    Point operator+(const Point& point) const { return {x + point.x, y + point.y, z + point.z}; }
    Point operator-(const Point& point) const { return {x - point.x, y - point.y, z - point.z}; }
    Point operator*(const double& val) const { return {x * val, y * val, z * val}; }
    Point operator/(const double& val) const {
        ULGUI_ASSERT(val != 0);
        return {x / val, y / val, z / val};
    }

    std::string toString() const {
        return "{x: " + std::to_string(x) + ", y: " + std::to_string(y) + ", z: " + std::to_string(z) + "}";
    }
};

} // namespace ULGui::base

#endif // _G_COORD_H
