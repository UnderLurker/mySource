//
// Created by XW on 2024/5/16.
//

#include "widget.h"

#include <cstring>

namespace graphics2D::window {
Widget::Widget(const Widget& obj) {
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

Widget::Widget(const GUint32& width, const GUint32& height, Widget* parent) {
    setWidth(width);
    setHeight(height);
    _parent = parent;
}

Widget::~Widget() {}

bool Widget::show() {
    if (!init()) return false;
    _window = glfwCreateWindow(Width(), Height(), _title.c_str(), nullptr, nullptr);
    if (!_window) return false;
    glfwMakeContextCurrent(_window);
    while(!glfwWindowShouldClose(_window)) {
        auto bgColor = Background();
        glClearColor(bgColor.red, bgColor.green, bgColor.blue, bgColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glColor4ub(255, 255, 255, 255);

        // 设置线的宽度
        glLineWidth(2.0f);

        // 绘制线时, 会将从 glBegin 到 glEnd 之间的所有的点都绘制出来
        // 可以调用 glVertex3f 方法 成对 设置多条线
        // 注意必须成对设置 , 如果设置奇数个点 , 最后一个点会被丢弃
        // 绘制线段开始
        glBegin(GL_LINES);

        // 绘制线 , 每两个点组成一条线
        // glVertex3f (GLfloat x, GLfloat y, GLfloat z)
        glVertex3f(0.0f, 0.0f, -10.0f);
        glVertex3f(-5.0f, 0.0f, -10.0f);

        // 上面的设置会从 (0,0,-10) 坐标向 (-5,0,-10) 坐标绘制一条线

        // 绘制点结束
        glEnd();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    return true;
}

void Widget::setTitle(char* title) {
    _title = std::string(title);
}

void Widget::setTitle(const std::string& title) {
    _title = title;
}

bool Widget::init() {
    // init
    return glfwInit();
}
} // namespace graphics2D::window
