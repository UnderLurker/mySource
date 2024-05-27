//
// Created by XW on 2024/5/16.
//

#ifndef _WIDGET_H

#include <GLFW/glfw3.h>
#include <map>
#include <memory>
#include <string>

#include "abstractComponent.h"
#include "abstractWidget.h"
#include "ULGuiType.h"

using namespace ULGui::components;

namespace ULGui::window {

class Widget : public virtual AbstractWidget {
public:
    Widget() = default;
    Widget(const Widget& obj);
    Widget(Widget&& obj) noexcept;
    explicit Widget(Widget* parent);
    explicit Widget(const GUint32& width, const GUint32& height, Widget* parent = nullptr);
    virtual ~Widget();


    bool show() override { return AbstractWidget::show(); };

private:
    // ID
    std::map<size_t, std::shared_ptr<AbstractComponent>> _childComponents;
};

} // namespace graphics2D::window
#endif // _WIDGET_H
