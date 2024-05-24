//
// Created by XW on 2024/5/16.
//

#ifndef _ABSTRACTWIDGET_H
#define _ABSTRACTWIDGET_H

#include "abstractAbility.h"
#include "graphic2DType.h"

namespace graphics2D {

class AbstractWidget : public virtual base::AbstractAbility {
public:
    GUint32 Width() const { return _width; }
    GUint32 Height() const { return _height; }
    GraphicRGBA Background() const { return _background; }
    void setWidth(const GUint32& width) {
        _width = width;
        setCoordSize(_width, _height);
    }
    void setHeight(const GUint32& height) {
        _height = height;
        setCoordSize(_width, _height);
    }
    void setBackground(const GraphicRGBA& background) { _background = background; }

private:
    GUint32 _width {WIDGET_WIDTH};
    GUint32 _height {WIDGET_HEIGHT};
    GraphicRGBA _background {1, 1, 1};
};

} // namespace graphics2D

#endif // _ABSTRACTWIDGET_H
