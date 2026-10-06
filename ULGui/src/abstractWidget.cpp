//
// Created by XW on 2024/5/27.
//
#include "abstractWidget.h"

#include "renderer.h"

namespace ULGui {
namespace {
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    base::Renderer::instance().setViewport(width, height);
}
} // namespace

void AbstractWidget::addChild(AbstractWidget* widget) {
    widget->setViewPortSize(width(), height());
    _childWidget[_childWidget.size()] = widget;
}

bool AbstractWidget::init() { return glfwInit(); }

bool AbstractWidget::show() {
    if (!init()) return false;

    // 现代管线：3.3 core profile（shader 必需）
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // 抗锯齿：请求多重采样 framebuffer（须在 glfwCreateWindow 之前设置）
    glfwWindowHint(GLFW_SAMPLES, _smooth ? DEFAULT_MSAA_SAMPLES : 0);

    _window = glfwCreateWindow(width(), height(), _title.c_str(), nullptr, nullptr);
    if (!_window) return false;
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1); // 用 vsync 同步帧率：拖动窗口时最平滑、CPU 占用最低（不再手动忙等节流）
    glfwSetWindowPos(_window, _location[0], _location[1]);
    glfwSetFramebufferSizeCallback(_window, &framebuffer_size_callback);

    // 初始化渲染后端：内部加载 glad、编译 shader、建 VAO/VBO
    auto& renderer = base::Renderer::instance();
    if (!renderer.init(width(), height(), reinterpret_cast<void* (*)(const char*)>(glfwGetProcAddress))) {
        return false;
    }

    while (!glfwWindowShouldClose(_window)) {
        updateFrameSize();

        renderer.clear(background());

        event::PaintEvent event;
        paintEvent(&event);

        for (const auto& item : _childWidget) {
            item.second->setViewPortSize(_size[0], _size[1]);
            item.second->setWidth(_size[0]);
            item.second->setHeight(_size[1]);
            item.second->paintEvent(&event);
        }

        renderer.flush();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    renderer.shutdown();
    return true;
}

void AbstractWidget::setTitle(char* title) { _title = std::string(title); }

void AbstractWidget::setTitle(const std::string& title) { _title = title; }

void AbstractWidget::updateFrameSize() {
    glfwGetWindowSize(_window, &_size[0], &_size[1]);
    setViewPortSize(_size[0], _size[1]);
    setWidth(_size[0]);
    setHeight(_size[1]);
}
} // namespace ULGui
