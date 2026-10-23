//
// Created by 常笑男 on 2024/7/20.
//

#ifndef _BRUSH_H
#define _BRUSH_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "ULGuiColor.h"

namespace ULGui::base {

// 画刷类型。渲染后端通过逐顶点采样渐变颜色来支持渐变画刷：
// 线性渐变在三角形内线性插值即可精确还原，径向渐变则依赖细分精度近似。
enum class BrushStyle : uint8_t {
    NoBrush,
    SolidColor,
    LinearGradient,
    RadialGradient,
};

class Gradient {
public:
    // 超出 [0,1] 的平铺方式
    enum class Spread : uint8_t {
        Pad,     // 拉伸到边界
        Repeat,  // 周期重复
        Reflect, // 镜像重复
    };

    Gradient()                           = default;
    Gradient(const Gradient&)            = default;
    Gradient(Gradient&&)                 = default;
    virtual ~Gradient()                  = default;
    Gradient& operator=(const Gradient&) = default;
    Gradient& operator=(Gradient&&)      = default;

    virtual BrushStyle style() const = 0;
    // 返回该点在归一化之前的原始渐变参数 t（可能越界，越界值交由 colorAt 按 spread 处理）
    virtual float positionAt(float x, float y) const = 0;

    void setSpread(Spread spread) { _spread = spread; }
    Spread spread() const { return _spread; }

    // position 会被 clamp 到 [0, 1]，stops 始终按 position 升序保存
    void setColorAt(float position, const RGBA& color);
    void clearStops() { _stops.clear(); }
    const std::vector<std::pair<float, RGBA>>& stops() const { return _stops; }

    // 取归一化位置 t 上的颜色（相邻 stop 线性插值，按 spread 处理越界）
    RGBA colorAt(float t) const;
    virtual std::string toString() const;

protected:
    std::string stopsToString() const;

private:

    Spread _spread {Spread::Pad};
    std::vector<std::pair<float, RGBA>> _stops;
};

class LinearGradient : public Gradient {
public:
    LinearGradient() = default;
    LinearGradient(float x0, float y0, float x1, float y1)
        : _x0(x0), _y0(y0), _x1(x1), _y1(y1) {}

    BrushStyle style() const override { return BrushStyle::LinearGradient; }
    float positionAt(float x, float y) const override;
    std::string toString() const override;

    void setStart(float x, float y) { _x0 = x, _y0 = y; }
    void setStop(float x, float y) { _x1 = x, _y1 = y; }
    float x0() const { return _x0; }
    float y0() const { return _y0; }
    float x1() const { return _x1; }
    float y1() const { return _y1; }

private:
    float _x0 {0}, _y0 {0};
    float _x1 {0}, _y1 {0};
};

// 径向渐变（中心 + 半径）
class RadiusGradient : public Gradient {
public:
    RadiusGradient() = default;
    RadiusGradient(float cx, float cy, float radius)
        : _cx(cx), _cy(cy), _radius(radius) {}

    BrushStyle style() const override { return BrushStyle::RadialGradient; }
    float positionAt(float x, float y) const override;
    std::string toString() const override;

    void setCenter(float x, float y) { _cx = x, _cy = y; }
    void setRadius(float radius) { _radius = radius; }
    float cx() const { return _cx; }
    float cy() const { return _cy; }
    float radius() const { return _radius; }

private:
    float _cx {0}, _cy {0};
    float _radius {0};
};

class ULBrush {
public:
    ULBrush() = default;
    ULBrush(const RGBA& color)
        : _color(color), _style(BrushStyle::SolidColor) {}
    explicit ULBrush(const std::shared_ptr<Gradient>& gradient)
        : _style(gradient ? gradient->style() : BrushStyle::SolidColor), _gradient(gradient) {}

    // 纯色画刷返回颜色；渐变画刷由 shader 逐像素采样，这里返回起始颜色作为占位
    RGBA color() const { return _gradient ? _gradient->colorAt(0.0f) : _color; }
    BrushStyle style() const { return _style; }
    const std::shared_ptr<Gradient>& gradient() const { return _gradient; }

    void setColor(const RGBA& color);
    void setGradient(const std::shared_ptr<Gradient>& gradient);

    // 纯色比较颜色；渐变按 gradient 指针同一性比较（渲染批处理据此判断是否需要 flush）
    bool operator==(const ULBrush& other) const;
    bool operator!=(const ULBrush& other) const { return !(*this == other); }
    std::string toString() const;

private:
    RGBA _color;
    BrushStyle _style {BrushStyle::SolidColor};
    std::shared_ptr<Gradient> _gradient;
};

} // namespace ULGui::base

#endif // _BRUSH_H
