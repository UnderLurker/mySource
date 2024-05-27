//
// Created by XW on 2024/5/16.
//

#include "widget.h"

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

Widget::Widget(const GUint32& width, const GUint32& height, Widget* parent){
    setWidth(width);
    setHeight(height);
}

Widget::~Widget() = default;

} // namespace graphics2D::window
