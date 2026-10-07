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
    widget.setBackground(ULGui::RGBA(122, 61, 61));
    widget.setLocation(100, 100);

    TriComponent tri;
    tri.setPoint({
        {300, 0 },
        base::Point(280, 200),
        {150,  20}});

    RectComponent rect;
    // auto linearGradient = std::make_shared<base::LinearGradient>(100.0f, 100.0f, 200.f, 200.f);
    // linearGradient->setSpread(base::Gradient::Spread::Repeat);
    // linearGradient->setColorAt(.0f, RGBA(255, 0, 0, 255));
    // linearGradient->setColorAt(1.0f, RGBA(0, 255, 0, 255));
    // rect.setBrush(base::ULBrush(linearGradient));
    auto radiusGradient = std::make_shared<base::RadiusGradient>(400.0f, 400.0f, 200.f);
    radiusGradient->setSpread(base::Gradient::Spread::Repeat);
    radiusGradient->setColorAt(.0f, RGBA(255, 0, 0, 255));
    radiusGradient->setColorAt(1.0f, RGBA(0, 255, 0, 255));
    rect.setBrush(base::ULBrush(radiusGradient));
    rect.setPosition({100, 100, 800, 800});

    widget.addChild(&tri);
    widget.addChild(&rect);
    widget.show();

    glfwTerminate();
    return 0;
}
