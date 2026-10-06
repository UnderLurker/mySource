
#include "ULGuiColor.h"

namespace ULGui {
const RGBA RGBA::BLACK       = RGBA(0, 0, 0, 255);
const RGBA RGBA::WHITE       = RGBA(255, 255, 255, 255);
const RGBA RGBA::RED         = RGBA(255, 0, 0, 255);
const RGBA RGBA::GREEN       = RGBA(0, 255, 0, 255);
const RGBA RGBA::BLUE        = RGBA(0, 0, 255, 255);
const RGBA RGBA::TRANSPARENT = RGBA(0, 0, 0, 0);


RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    _data   = a;
    _data <<= 8;
    _data  |= r;
    _data <<= 8;
    _data  |= g;
    _data <<= 8;
    _data  |= b;
}
uint8_t RGBA::red() const { return (_data >> 16) & 0xFF; }
uint8_t RGBA::green() const { return (_data >> 8) & 0xFF; }
uint8_t RGBA::blue() const { return _data & 0xFF; }
uint8_t RGBA::alpha() const { return (_data >> 24) & 0xFF; }

std::string RGBA::toString() const {
    constexpr char HEX[]         = "0123456789ABCDEF";
    constexpr uint32_t stringLen = 9;
    std::string res(stringLen, '#');
    for (uint32_t i = 0; i < stringLen - 1; i++) {
        uint32_t index         = (_data >> (4 * i)) & 0xF;
        res[stringLen - 1 - i] = HEX[index];
    }
    return std::move(res);
}
RGBA& RGBA::operator=(const uint32_t& color) {
    _data = color;
    return *this;
}
RGBA RGBA::makeRGBA(const std::string& str) {
    constexpr uint32_t stringLen = 9;
    if ((str.size() != stringLen && str.size() != stringLen - 2) || str[0] != '#') { return {}; }
    uint32_t data             = 0;
    constexpr uint32_t offset = 4;
    for (uint32_t i = 1; i < str.size(); i++) {
        data <<= offset;
        data  += str[i] < 'A' ? str[i] - '0' : str[i] - '7';
    }
    return RGBA(data);
}
} // namespace ULGui
