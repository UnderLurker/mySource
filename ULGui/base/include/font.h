//
// Created by 常笑男 on 2024/7/20.
//

#ifndef _FONT_H
#define _FONT_H

#include <cstdint>

#include "dimension.h"
#include "ULGuiType.h"

namespace ULGui::base {

#define FONT_DEFAULT_FAMILY ""

enum FontWeight : size_t {
    Normal = 0
};

class ULFont {
public:
    ULFont() = default;

    FontWeight weight() const { return _weight; }
    Dimension size() const { return _size; }
    std::string fontFamily() const { return _fontFamily; };
    GraphicRGBA color() const { return _color; }
    void setWeight(FontWeight weight) { _weight = weight; }
    void setSize(const Dimension& size) { _size = size; }
    void setSize(Dimension&& size) { _size = size; }
    void setFontFamily(const std::string& fontFamily) { _fontFamily = fontFamily; }
    void setColor(GraphicRGBA color) { _color = color; }

private:
    FontWeight _weight { Normal };
    Dimension _size {0};
    std::string _fontFamily { FONT_DEFAULT_FAMILY };
    GraphicRGBA _color { DEFAULT_PEN_COLOR };
};

} // namespace ULGui::base

#endif // _FONT_H
