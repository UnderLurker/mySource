//
// Created by cxn on 2026/10/5.
//
#ifndef _ULGUI_COLOR_H
#define _ULGUI_COLOR_H

#include <cstdint>
#include "ULGuiMacro.h"
#include <string>

namespace ULGui {

// format
//      uint32_t: argb
//      string  : #argb
__ULGUI_DECLARE class RGBA {
public:
    RGBA()            = default;
    RGBA(const RGBA&) = default;
    RGBA(RGBA&&)      = default;
    explicit RGBA(uint32_t color)
        : _data(color) {}
    explicit RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    ~RGBA() = default;
    uint8_t red() const;
    uint8_t green() const;
    uint8_t blue() const;
    uint8_t alpha() const;

    std::string toString() const;
    explicit operator uint32_t() const { return _data; }
    RGBA& operator=(const uint32_t& color);
    RGBA& operator=(const RGBA& color) = default;
    RGBA& operator=(RGBA&& color)      = default;

    static RGBA makeRGBA(const std::string& str);

public:
    static const RGBA BLACK;
    static const RGBA WHITE;
    static const RGBA RED;
    static const RGBA GREEN;
    static const RGBA BLUE;
    static const RGBA TRANSPARENT;

private:
    uint32_t _data = DEFAULT_RGBA_COLOR;
};
} // namespace ULGui

#endif // !_ULGUI_COLOR_H
