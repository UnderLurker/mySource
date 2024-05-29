//
// Created by XW on 2024/5/16.
//
#include <thread>

#include "baseGraphics.h"
#include "widget.h"

int main() {
    ULGui::window::Widget widget(600, 300);
    std::string str = "this is a title";
    widget.setTitle(str);
    widget.setBackground(ULGui::GraphicRGBA(122, 61, 61));
    widget.setLocation(100, 100);
    widget.show();

    glfwTerminate();
    return 0;
}
