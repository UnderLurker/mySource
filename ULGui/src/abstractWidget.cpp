//
// Created by XW on 2024/5/27.
//
#include "abstractWidget.h"

#include <functional>
#include <GL/gl.h>

namespace ULGui {
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

bool AbstractWidget::init() { return glfwInit(); }

bool AbstractWidget::show() {
    if (!init()) return false;
    _window = glfwCreateWindow(width(), height(), _title.c_str(), nullptr, nullptr);
    if (!_window) return false;
    glfwMakeContextCurrent(_window);
    glfwSetWindowPos(_window, _location[0], _location[1]);
    glfwSetFramebufferSizeCallback(_window, &framebuffer_size_callback);
    setSmooth();
    while (!glfwWindowShouldClose(_window)) {
        updateFrameSize();

        auto bgColor = background();
        glClearColor(bgColor.red, bgColor.green, bgColor.blue, bgColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        event::PaintEvent event;
        paintEvent(&event);

        glFlush();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    return true;
}

void AbstractWidget::setTitle(char* title) { _title = std::string(title); }

void AbstractWidget::setTitle(const std::string& title) { _title = title; }

void AbstractWidget::setSmooth() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void AbstractWidget::updateFrameSize() {
    glfwGetWindowSize(_window, &_size[0], &_size[1]);
    setCoordSize(_size[0], _size[1]);
    setWidth(_size[0]);
    setHeight(_size[1]);
}
} // namespace ULGui
