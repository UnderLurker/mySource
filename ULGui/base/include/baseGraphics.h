//
// Created by XW on 2024/5/16.
//

#ifndef _BASE_GRAPHICS_H
#define _BASE_GRAPHICS_H

#include <cmath>
#include <utility>

#include "abstractAbility.h"
#include "paintEvent.h"
#include "ULGuiType.h"

namespace ULGui::base {
using TriPosition  = GVec<Point, 3>;
using RectPosition = GVec4f;

class BaseGraphic : public virtual AbstractAbility {
public:
    BaseGraphic()          = default;
    virtual ~BaseGraphic() = default;
    virtual void paintEvent(event::PaintEvent* event) {}
    bool fill {false};
};

class Triangle : public BaseGraphic {
public:
    Triangle() = default;
    explicit Triangle(const TriPosition& pos)
        : _pos(pos) {}
    const TriPosition& point() const { return _pos; }
    void setPoint(const TriPosition& point) { _pos = point; }
    void paintEvent(event::PaintEvent* event) override;

private:
    TriPosition _pos;
};

class Rectangle : public BaseGraphic {
public:
    Rectangle() = default;
    explicit Rectangle(const RectPosition& pos)
        : top(pos[0]), right(pos[1]), bottom(pos[2]), left(pos[3]) {}
    RectPosition position() const { return RectPosition {top, right, bottom, left}; }
    void setPosition(const RectPosition& pos);
    void paintEvent(event::PaintEvent* event) override;

    explicit operator GVec4f() const noexcept { return GVec4f {top, right, bottom, left}; }

public:
    float top {0};
    float left {0};
    float bottom {0};
    float right {0};
};

class Circle : public BaseGraphic {
public:
    Circle() = default;
    explicit Circle(const Point& c, float r, float start = 0, float end = 360)
        : center(c), radius(r), startAngle(start), endAngle(end) {}
    void paintEvent(event::PaintEvent* event) override;

public:
    Point center {0, 0};
    float radius {0};
    float startAngle {0};
    float endAngle {0};
};
} // namespace ULGui::base

#endif // _BASE_GRAPHICS_H
