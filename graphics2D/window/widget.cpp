//
// Created by XW on 2024/5/16.
//

#include "widget.h"

#include <cstring>

namespace graphics2D::window {
Widget::Widget(const Widget& obj)  : AbstractAbility(obj), AbstractWidget(obj) {
    setWidth(obj.Width());
    setHeight(obj.Height());
    _parent = obj._parent;
}

Widget::Widget(Widget&& obj) noexcept {
    setWidth(obj.Width());
    setHeight(obj.Height());
    _parent     = obj._parent;
    obj._parent = nullptr;
}

Widget::Widget(Widget* parent) {
    if (parent) _parent = parent;
}

Widget::Widget(const GUint32& width, const GUint32& height, Widget* parent){
    setWidth(width);
    setHeight(height);
    _parent = parent;
}

Widget::~Widget() = default;

bool Widget::show() {
    if (!init()) return false;
    _window = glfwCreateWindow(Width(), Height(), _title.c_str(), nullptr, nullptr);
    if (!_window) return false;
    glfwMakeContextCurrent(_window);
    while (!glfwWindowShouldClose(_window)) {
        auto bgColor = Background();
        glClearColor(bgColor.red, bgColor.green, bgColor.blue, bgColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setPenWidth(5);
        lineTo({20, 0}, base::Point(Width(), Height()));
        point(base::Point{10, 20});

        glFlush();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    return true;
}

void Widget::setTitle(char* title) { _title = std::string(title); }

void Widget::setTitle(const std::string& title) { _title = title; }

bool Widget::init() {
    // init
    return glfwInit();
}
} // namespace graphics2D::window
