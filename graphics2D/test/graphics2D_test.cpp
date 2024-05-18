//
// Created by XW on 2024/5/16.
//
#include <thread>

#include "base_graphics.h"
#include "widget.h"

int main() {
    graphics2D::base::Rectangle a;
    graphics2D::window::Widget widget(600, 300);
    std::string str = "this is a title";
    widget.setTitle(str);
    widget.setBackground(graphics2D::GraphicRGBA(1, 0.2, 0.3));
    widget.show();

    glfwTerminate();
    return 0;
}
