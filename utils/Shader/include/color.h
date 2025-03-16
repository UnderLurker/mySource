//
// Created by 常笑男 on 2025/3/16.
//

#ifndef _COLOR_H_
#define _COLOR_H_

#include <cstdint>
#include <vector>

namespace myUtil {

enum ColorLevel : uint16_t {
    LOW,
    HIGH
};

struct ColorParam {
    uint16_t alpha : 16;
    uint16_t red   : 16;
    uint16_t green : 16;
    uint16_t blue  : 16;
    ColorLevel level {LOW};
    std::vector<float> convertFloat() const {
        if (level == myUtil::ColorLevel::LOW) return {red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f};
        return {red / 65535.0f, green / 65535.0f, blue / 65535.0f, alpha / 65535.0f};
    }
};

using Color = ColorParam;
const Color WHITE {.alpha = 255, .red = 255, .green = 255, .blue = 255};
const Color RED {.alpha = 255, .red = 255, .green = 0, .blue = 0};
const Color GREEN {.alpha = 255, .red = 0, .green = 255, .blue = 0};
const Color BLUE {.alpha = 255, .red = 0, .green = 0, .blue = 255};
const Color BLACK {.alpha = 255, .red = 0, .green = 0, .blue = 0};
const Color TRANSPARENT {.alpha = 0, .red = 0, .green = 0, .blue = 0};
} // namespace myUtil

#endif // _COLOR_H_
