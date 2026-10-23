//
// Created by 常笑男 on 2024/7/20.
//
#include "brush.h"

namespace ULGui::base {
namespace {
static const char* SpreadName(Gradient::Spread spread) {
    switch (spread) {
        case Gradient::Spread::Pad:
            return "Pad";
        case Gradient::Spread::Repeat:
            return "Repeat";
        case Gradient::Spread::Reflect:
            return "Reflect";
    }
    return "Unknown";
}

static float normalize(float t, Gradient::Spread spread) {
    if (spread == Gradient::Spread::Repeat) {
        const float f = std::fmod(t, 1.0f);
        return f < 0.0f ? f + 1.0f : f;
    }
    if (spread == Gradient::Spread::Reflect) {
        const float f = std::fmod(std::fabs(t), 2.0f);
        return f > 1.0f ? 2.0f - f : f;
    }
    return std::max(0.0f, std::min(1.0f, t)); // Pad
}

static RGBA mix(const RGBA& from, const RGBA& to, float t) {
    auto channel = [t](uint8_t a, uint8_t b) { return static_cast<uint8_t>(std::lround(a + (b - a) * t)); };
    return RGBA(channel(from.red(), to.red()), channel(from.green(), to.green()), channel(from.blue(), to.blue()),
                channel(from.alpha(), to.alpha()));
}

} // namespace

void Gradient::setColorAt(float position, const RGBA& color) {
    const float p = std::max(0.0f, std::min(1.0f, position));
    _stops.emplace_back(p, color);
    std::sort(_stops.begin(), _stops.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
}

RGBA Gradient::colorAt(float t) const {
    if (_stops.empty()) return {};
    const float pos = normalize(t, _spread);
    if (pos <= _stops.front().first) return _stops.front().second;
    for (size_t i = 1; i < _stops.size(); ++i) {
        if (pos <= _stops[i].first) {
            const auto& lo    = _stops[i - 1];
            const auto& hi    = _stops[i];
            const float span  = hi.first - lo.first;
            const float ratio = span <= 0.0f ? 0.0f : (pos - lo.first) / span;
            return mix(lo.second, hi.second, ratio);
        }
    }
    return _stops.back().second;
}

std::string Gradient::toString() const {
    return std::string("Gradient{spread=") + SpreadName(_spread) + ", stops=" + stopsToString() + "}";
}
std::string Gradient::stopsToString() const {
    std::string out = "[";
    for (size_t i = 0; i < _stops.size(); ++i) {
        if (i > 0) out += ", ";
        out += "(" + std::to_string(_stops[i].first) + ", " + _stops[i].second.toString() + ")";
    }
    out += "]";
    return out;
}

float LinearGradient::positionAt(float x, float y) const {
    const float dx   = _x1 - _x0;
    const float dy   = _y1 - _y0;
    const float len2 = dx * dx + dy * dy;
    if (len2 <= 0.0f) return 0.0f;
    return ((x - _x0) * dx + (y - _y0) * dy) / len2;
}

std::string LinearGradient::toString() const {
    return std::string("LinearGradient{from=(") + std::to_string(_x0) + ", " + std::to_string(_y0) + "), to=(" +
           std::to_string(_x1) + ", " + std::to_string(_y1) + "), spread=" + SpreadName(spread()) +
           ", stops=" + stopsToString() + "}";
}

float RadiusGradient::positionAt(float x, float y) const {
    if (_radius <= 0.0f) return 0.0f;
    const float dx = x - _cx;
    const float dy = y - _cy;
    return std::sqrt(dx * dx + dy * dy) / _radius;
}

std::string RadiusGradient::toString() const {
    return std::string("RadialGradient{center=(") + std::to_string(_cx) + ", " + std::to_string(_cy) +
           "), radius=" + std::to_string(_radius) + ", spread=" + SpreadName(spread()) + ", stops=" + stopsToString() +
           "}";
}

void ULBrush::setColor(const RGBA& color) {
    _color = color;
    _style = BrushStyle::SolidColor;
    _gradient.reset();
}

void ULBrush::setGradient(const std::shared_ptr<Gradient>& gradient) {
    _style    = gradient ? gradient->style() : BrushStyle::SolidColor;
    _gradient = gradient;
}

// 纯色比较颜色；渐变按 gradient 指针同一性比较（渲染批处理据此判断是否需要 flush）
bool ULBrush::operator==(const ULBrush& other) const {
    if (_style != other._style) return false;
    if (_style == BrushStyle::SolidColor || _style == BrushStyle::NoBrush)
        return static_cast<uint32_t>(_color) == static_cast<uint32_t>(other._color);
    return _gradient == other._gradient;
}

std::string ULBrush::toString() const {
    switch (_style) {
        case BrushStyle::NoBrush:
            return "ULBrush{NoBrush}";
        case BrushStyle::SolidColor:
            return std::string("ULBrush{SolidColor, ") + _color.toString() + "}";
        case BrushStyle::LinearGradient:
        case BrushStyle::RadialGradient:
            return std::string("ULBrush{") + (_gradient ? _gradient->toString() : std::string("<null gradient>")) + "}";
    }
    return "ULBrush{Unknown}";
}
} // namespace ULGui::base
