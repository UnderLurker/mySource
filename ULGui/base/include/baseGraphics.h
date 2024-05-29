//
// Created by XW on 2024/5/16.
//

#ifndef _BASE_GRAPHICS_H
#define _BASE_GRAPHICS_H

#include <utility>

#include "abstractAbility.h"
#include "paintEvent.h"
#include "ULGuiType.h"

namespace ULGui::base {
using TriPosition = GVec<Point, 3>;

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
    void paintEvent(event::PaintEvent* event) override;

    explicit operator GVec4f() const noexcept { return GVec4f {top, left, bottom, right}; }

public:
    float top {0};
    float left {0};
    float bottom {0};
    float right {0};
};

} // namespace ULGui::base

#endif // _BASE_GRAPHICS_H
