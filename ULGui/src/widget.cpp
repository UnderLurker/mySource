//
// Created by XW on 2024/5/16.
//

#include "widget.h"
#include "baseGraphics.h"

#include <cstring>

namespace ULGui::window {
Widget::Widget(const Widget& obj)  : AbstractAbility(obj), AbstractWidget(obj) {
    setWidth(obj.width());
    setHeight(obj.height());
    setParent(obj.parent());
}

Widget::Widget(Widget&& obj) noexcept {
    setWidth(obj.width());
    setHeight(obj.height());
    setParent(obj.parent());
    obj.setParent(nullptr);
}

Widget::Widget(Widget* parent) {
    if (parent) setParent(parent);
}

Widget::Widget(const GInt32& width, const GInt32& height, Widget* parent){
    setWidth(width);
    setHeight(height);
}

Widget::~Widget() = default;

void Widget::paintEvent(event::PaintEvent* event) {
    setPenWidth(1);
    setStippleStyle({4, 0xAAAA});
    setLineType(base::Stipple);
    setLineMode(base::LINE_LOOP);
    lineTo({
        {20, 0},
        base::Point(width() / 2 + 10, height() / 2)
    });
    lineTo({
        base::Point(width() / 8, height() / 2), {0, 20}
    });
    lineTo({
        {40, 0 },
        base::Point(width() / 4, height() / 2),
        {0,  20}
    });
    point(base::Point {10, 20});
    base::Triangle tri({
        {80, 0 },
        base::Point(width() / 4, height() / 2),
        {20,  20}});
    tri.setCoordSize(width(), height());
    tri.paintEvent(event);

    base::Circle circle({width() / 2.0, height() / 2.0}, height() / 2.0, 0, 20);
    circle.setPenWidth(0.5);
    circle.setCoordSize(width(), height());
    circle.paintEvent(event);
}

} // namespace graphics2D::window
