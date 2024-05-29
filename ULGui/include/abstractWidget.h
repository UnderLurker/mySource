//
// Created by XW on 2024/5/16.
//

#ifndef _ABSTRACTWIDGET_H
#define _ABSTRACTWIDGET_H

#include <GLFW/glfw3.h>
#include <map>
#include <memory>
#include <string>

#include "abstractAbility.h"
#include "paintEvent.h"
#include "ULGuiType.h"

namespace ULGui {

class AbstractWidget : public virtual base::AbstractAbility {
public:
    GInt32 width() const { return _size[0]; }
    GInt32 height() const { return _size[1]; }
    GInt32 x() const { return _location[0]; }
    GInt32 y() const { return _location[1]; }
    GVec2i location() const { return _location; }
    GraphicRGBA background() const { return _background; }
    /**
     * @return { width, height }
     */
    GVec2i size() const { return _size; }
    bool smooth() const { return _smooth; }
    AbstractWidget* parent() const { return _parent; }
    const char* title() const { return _title.c_str(); }

    void setWidth(const GInt32& width) { _size[0] = width, setCoordSize(_size[0], _size[1]); }
    void setHeight(const GInt32& height) { _size[1] = height, setCoordSize(_size[0], _size[1]); }
    void setX(const GInt32& xPos) { _location[0] = xPos; }
    void setY(const GInt32& yPos) { _location[1] = yPos; }
    void setLocation(const GInt32& xPos, const GInt32& yPos) { _location[0] = xPos, _location[1] = yPos; }
    void setBackground(const GraphicRGBA& background) { _background = background; }
    /**
     * @param size { width, height }
     */
    void resize(const GVec2i& size) { _size[0] = size[0], _size[1] = size[1]; }
    void setSmooth(bool smooth) { _smooth = smooth; }
    void setParent(AbstractWidget* parent) { _parent = parent; }
    void setTitle(char* title);
    void setTitle(const std::string& title);

    void addChild(AbstractWidget* widget);

    bool init();
    virtual bool show();
    virtual void paintEvent(event::PaintEvent* event) {}

protected:
    static void setSmooth();
    void updateFrameSize();

private:
    void stableFrameRate();
    static void _sleep(double time);

private:
    AbstractWidget* _parent {nullptr};
    std::string _title {DEFAULT_WIDGET_TITLE};
    GLFWwindow* _window {nullptr};
    GVec2i _size {DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT}; // {width, height}
    GVec2i _location {0, 0};                                    // {left, top} or {x, y}
    GraphicRGBA _background {255, 255, 255};
    bool _smooth {true};
    std::map<size_t, AbstractWidget*> _childWidget;
    double _lastime {0};
};

} // namespace ULGui

#endif // _ABSTRACTWIDGET_H
