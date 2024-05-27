//
// Created by XW on 2024/5/27.
//
#include "abstractWidget.h"

#include <GL/gl.h>

namespace ULGui {
bool AbstractWidget::init() {
    return glfwInit();
}

bool AbstractWidget::show() {
    if (!init()) return false;
    _window = glfwCreateWindow(width(), height(), _title.c_str(), nullptr, nullptr);
    if (!_window) return false;
    glfwMakeContextCurrent(_window);
    glEnable ( GL_DEPTH_TEST );
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(_window)) {
        auto bgColor = background();
        glClearColor(bgColor.red, bgColor.green, bgColor.blue, bgColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setPenWidth(1);
        lineTo({20, 0}, base::Point(width(), height()));
        point(base::Point{10, 20});

        glFlush();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    return true;
}

void AbstractWidget::setTitle(char* title) { _title = std::string(title); }

void AbstractWidget::setTitle(const std::string& title) { _title = title; }
}
