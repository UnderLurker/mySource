//
// Created by 常笑男 on 2024/7/20.
//

#ifndef _DIMENSION_H
#define _DIMENSION_H

#include <cassert>
#include <string>

namespace ULGui::base {

#define DIMENSION_DEFAULT_NUMBER 0.0
#define DIMENSION_DEFAULT_UNIT   None
#define DIMENSION_DEFAULT_VALUE  DIMENSION_DEFAULT_NUMBER #DIMENSION_DEFAULT_UNIT

enum DimensionUnit {
    None = 0,
};

class Dimension {
public:
    Dimension() = default;
    constexpr explicit Dimension(float value, DimensionUnit unit = DIMENSION_DEFAULT_UNIT)
        : _value(value), _unit(unit) {}

    float value() const { return _value; }
    void setNumber(float value) { _value = value; }

    DimensionUnit unit() const { return _unit; }
    void setUnit(DimensionUnit unit) { _unit = unit; }

    bool operator==(const Dimension& obj) const { return obj._value == _value && obj._unit == _unit; }

    bool operator!=(const Dimension& obj) const { return !(*this == obj); }

    bool operator>(const Dimension& obj) const {
        assert(_unit == obj._unit);
        return _value > obj._value;
    }

    bool operator<(const Dimension& obj) const {
        assert(_unit == obj._unit);
        return _value < obj._value;
    }

    constexpr Dimension operator/(float value) const {
        if (value == 0) return {};
        return Dimension {_value / value, _unit};
    }

    constexpr Dimension operator*(float value) const { return Dimension {_value * value, _unit}; }

    constexpr Dimension operator-() const { return Dimension {-_value, _unit}; }

    constexpr Dimension operator+(const Dimension& obj) const {
        assert(_unit == obj._unit);
        return Dimension {_value + obj._value, _unit};
    }

    Dimension& operator+=(const Dimension& obj) {
        assert(_unit == obj._unit);
        _value += obj._value;
        return *this;
    }

    constexpr Dimension operator-(const Dimension& obj) const {
        assert(_unit == obj._unit);
        return Dimension {_value - obj._value, _unit};
    }

    Dimension& operator-=(const Dimension& obj) {
        assert(_unit == obj._unit);
        _value -= obj._value;
        return *this;
    }

    std::string toString() { return std::to_string(_value) + convertUnitToString(_unit); }

    static Dimension fromString(const std::string& str) {
        char* pEnd  = nullptr;
        auto result = std::strtof(str.c_str(), &pEnd);
        if (pEnd == str.c_str()) return Dimension {DIMENSION_DEFAULT_NUMBER, DIMENSION_DEFAULT_UNIT};
        return Dimension {result, convertStringToUnit(str)};
    }

    static std::string convertUnitToString(const DimensionUnit& unit) {
        switch (unit) {
            case None:
            default:
                return "";
        }
    }

    static DimensionUnit convertStringToUnit(const std::string& str) { return None; }

private:
    float _value {DIMENSION_DEFAULT_NUMBER};
    DimensionUnit _unit {DIMENSION_DEFAULT_UNIT};
};

} // namespace ULGui::base

#endif // _DIMENSION_H
