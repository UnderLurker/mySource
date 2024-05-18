//
// Created by XW on 2024/5/16.
//

#ifndef _WIDGET_H

#include <GLFW/glfw3.h>
#include <string>

#include "abstractWidget.h"
#include "graphic2DType.h"

namespace graphics2D::window {

class Widget : public virtual AbstractWidget {
public:
    Widget() = delete;
    Widget(const Widget& obj);
    Widget(Widget&& obj) noexcept;
    explicit Widget(Widget* parent);
    explicit Widget(const GUint32& width, const GUint32& height, Widget* parent = nullptr);
    virtual ~Widget();

    const Widget* Parent() const { return _parent; }
    const char* Title() const { return _title.c_str(); }
    void setParent(Widget* parent) { _parent = parent; }
    void setTitle(char* title);
    void setTitle(const std::string& title);

    bool show();

protected:
    bool init();

private:
    Widget* _parent {nullptr};
    std::string _title {WIDGET_TITLE};
    GLFWwindow* _window {nullptr};
};

} // namespace graphics2D::window
#endif // _WIDGET_H
