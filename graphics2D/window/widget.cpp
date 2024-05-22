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

        // �����ߵĿ��
        glLineWidth(2.0f);

        // ������ʱ, �Ὣ�� glBegin �� glEnd ֮������еĵ㶼���Ƴ���
        // ���Ե��� glVertex3f ���� �ɶ� ���ö�����
        // ע�����ɶ����� , ��������������� , ���һ����ᱻ����
        // �����߶ο�ʼ
        glBegin(GL_LINES);

        // ������ , ÿ���������һ����
        // glVertex3f (GLfloat x, GLfloat y, GLfloat z)
        glVertex3f(0.0f, 0.0f, -10.0f);
        glVertex3f(-5.0f, 0.0f, -10.0f);

        // ��������û�� (0,0,-10) ������ (-5,0,-10) �������һ����

        // ���Ƶ����
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
