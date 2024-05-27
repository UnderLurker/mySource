//
// Created by XW on 2024/5/16.
//
#include <thread>

#include "baseGraphics.h"
#include "widget.h"

int main() {
//    graphics2D::base::Rectangle a;
    ULGui::window::Widget widget(600, 300);
    std::string str = "this is a title";
    widget.setTitle(str);
    widget.setBackground(ULGui::GraphicRGBA(0.5, 0.2, 0.3));
    widget.show();

    glfwTerminate();
    return 0;
}
