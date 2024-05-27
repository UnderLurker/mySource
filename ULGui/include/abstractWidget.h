//
// Created by XW on 2024/5/16.
//

#ifndef _ABSTRACTWIDGET_H
#define _ABSTRACTWIDGET_H

#include <GLFW/glfw3.h>
#include <string>

#include "abstractAbility.h"
#include "ULGuiType.h"

namespace ULGui {

class AbstractWidget : public virtual base::AbstractAbility {
public:
    GUint32 width() const { return _width; }
    GUint32 height() const { return _height; }
    GraphicRGBA background() const { return _background; }
    /**
     * @return { width, height }
     */
    GVec2u size() const { return {_width, _height}; }
    bool smooth() const { return _smooth; }
    AbstractWidget* parent() const { return _parent; }
    const char* title() const { return _title.c_str(); }
    void setWidth(const GUint32& width) { _width = width, setCoordSize(_width, _height); }
    void setHeight(const GUint32& height) { _height = height, setCoordSize(_width, _height); }
    void setBackground(const GraphicRGBA& background) { _background = background; }
    /**
     * @param size { width, height }
     */
    void resize(const GVec2u& size) { _width = size[0], _height = size[1]; }
    void setSmooth(bool smooth) { _smooth = smooth; }
    void setParent(AbstractWidget* parent) { _parent = parent; }
    void setTitle(char* title);
    void setTitle(const std::string& title);

    bool init();
    virtual bool show();

private:
    AbstractWidget* _parent {nullptr};
    std::string _title {WIDGET_TITLE};
    GLFWwindow* _window {nullptr};
    GUint32 _width {WIDGET_WIDTH};
    GUint32 _height {WIDGET_HEIGHT};
    GraphicRGBA _background {1, 1, 1};
    bool _smooth{true};

};

} // namespace ULGui

#endif // _ABSTRACTWIDGET_H
