//
// Created by XW on 2024/5/16.
//
#include <thread>

#include "baseGraphics.h"
#include "widget.h"
#include "abstractWidget.h"

using namespace ULGui;
class TriComponent : public virtual base::Triangle, public virtual AbstractWidget {
public:
    void paintEvent(event::PaintEvent *event) override {
        base::Triangle::paintEvent(event);
    }
};

class RectComponent : public virtual base::Rectangle, public virtual AbstractWidget {
public:
    void paintEvent(event::PaintEvent *event) override {
        base::Rectangle::paintEvent(event);
    }
};

int main() {
    ULGui::window::Widget widget(600, 300);
    std::string str = "this is a title";
    widget.setTitle(str);
    widget.setBackground(ULGui::GraphicRGBA(122, 61, 61));
    widget.setLocation(100, 100);

    TriComponent tri;
    tri.setPoint({
        {300, 0 },
        base::Point(280, 200),
        {150,  20}});

    RectComponent rect;
    rect.setPosition({100, 500, 290, 300});

    widget.addChild(&tri);
    widget.addChild(&rect);
    widget.show();

    glfwTerminate();
    return 0;
}
