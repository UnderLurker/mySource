//
// Created by 常笑男 on 2024/7/20.
//

#ifndef _FONT_H
#define _FONT_H

#include <cstdint>

#include "dimension.h"

namespace ULGui::base {

#define FONT_DEFAULT_FAMILY ""

enum FontWeight : size_t {
    Normal = 0
};

class ULFont {
private:
    FontWeight _weight { Normal };
    Dimension _size {0};
    std::string _fontFamily { FONT_DEFAULT_FAMILY };
};

} // namespace ULGui::base

#endif // _FONT_H
