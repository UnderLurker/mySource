//
// Created by XW on 2024/5/16.
//

#ifndef _GRAPHIC2DTYPE_H
#define _GRAPHIC2DTYPE_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <sstream>
#include <string>

#include "ULGuiMacro.h"

namespace ULGui {

__ULGUI_2D_DECLARE typedef signed char GInt8;
__ULGUI_2D_DECLARE typedef unsigned char GUint8;
__ULGUI_2D_DECLARE typedef short GInt16;
__ULGUI_2D_DECLARE typedef unsigned short GUint16;
__ULGUI_2D_DECLARE typedef int GInt32;
__ULGUI_2D_DECLARE typedef unsigned GUint32;
__ULGUI_2D_DECLARE typedef long long GInt64;
__ULGUI_2D_DECLARE typedef unsigned long long GUint64;

template<typename T, size_t size>
__ULGUI_DECLARE class GVec {
public:
    GVec() = default;
    GVec(const GVec<T, size>& vec) {
        delete[] _ptr;
        _size = size;
        _ptr  = new T[_size];
        memcpy(_ptr, vec._ptr, sizeof(T) * _size);
    }
    GVec(GVec<T, size>&& vec) noexcept {
        delete[] _ptr;
        _ptr      = vec._ptr;
        _size     = size;
        vec._size = 0;
        vec._ptr  = nullptr;
    }
    explicit GVec(const T& val)
        : _size(size) {
        if (size == 0) return;
        _ptr = new T[size] {val};
    }
    GVec(const std::initializer_list<T>& list)
        : _size(size) {
        if (list.size() == 0 || list.size() != size) return;
        _ptr = new T[_size];
        memcpy(_ptr, list.begin(), sizeof(T) * _size);
    }
    ~GVec() {
        delete[] _ptr;
        _ptr = nullptr;
    }

    const T& at(const size_t& idx) const {
        assert(idx >= 0 && idx < _size);
        return _ptr[idx];
    }
    T& operator[](const size_t& idx) const {
        assert(idx >= 0 && idx < _size);
        return _ptr[idx];
    }

    GVec<T, size>& operator=(const GVec<T, size>& vec) {
        delete[] _ptr;
        _ptr = new T[_size];
        memcpy(_ptr, vec._ptr, sizeof(T) * _size);
        return *this;
    }

    const T* rawPtr() const { return _ptr; }

    std::string toString() const {
        std::ostringstream tmp;
        tmp << "{";
        for(size_t i = 0; i < _size; i++) {
            if constexpr (std::is_arithmetic_v<T>) {
                tmp << std::to_string(_ptr[i]);
            } else {
                tmp << _ptr[i].toString();
            }
            if (i != _size - 1) {
                tmp << ',';
            }
        }
        tmp << "}";
        return tmp.str();
    }

private:
    T* _ptr {nullptr};
    size_t _size {size};
};

__ULGUI_DECLARE typedef GVec<GUint32, 2> GVec2u;
__ULGUI_DECLARE typedef GVec<GUint32, 3> GVec3u;
__ULGUI_DECLARE typedef GVec<GUint32, 4> GVec4u;
__ULGUI_DECLARE typedef GVec<GInt32, 2> GVec2i;
__ULGUI_DECLARE typedef GVec<GInt32, 3> GVec3i;
__ULGUI_DECLARE typedef GVec<GInt32, 4> GVec4i;
__ULGUI_DECLARE typedef GVec<float, 2> GVec2f;
__ULGUI_DECLARE typedef GVec<float, 3> GVec3f;
__ULGUI_DECLARE typedef GVec<float, 4> GVec4f;
__ULGUI_DECLARE typedef GVec<double, 2> GVec2d;
__ULGUI_DECLARE typedef GVec<double, 3> GVec3d;
__ULGUI_DECLARE typedef GVec<double, 4> GVec4d;

// format
//      uint32_t: argb
//      string  : #argb
__ULGUI_DECLARE class GraphicRGBA {
public:
    GraphicRGBA() = default;
    GraphicRGBA(const GraphicRGBA&) = default;
    GraphicRGBA(GraphicRGBA&&) = default;
    explicit GraphicRGBA(uint32_t color) : _data(color) {}
    explicit GraphicRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        _data = a;
        _data <<= 8;
        _data |= r;
        _data <<= 8;
        _data |= g;
        _data <<= 8;
        _data |= b;
    }
    uint8_t red() const { return (_data >> 16) & 0xFF; }
    uint8_t green() const { return (_data >> 8) & 0xFF; }
    uint8_t blue() const { return _data & 0xFF; }
    uint8_t alpha() const { return (_data >> 24) & 0xFF; }

    std::string toString() const {
        constexpr char HEX[] = "0123456789ABCDEF";
        constexpr uint32_t stringLen = 9;
        std::string res(stringLen, '#');
        for(uint32_t i = 0; i < stringLen - 1; i++) {
            uint32_t index = (_data >> (4 * i)) & 0xF;
            res[stringLen - 1 - i] = HEX[index];
        }
        return std::move(res);
    }
    explicit operator uint32_t() const {
        return _data;
    }
    GraphicRGBA& operator=(const uint32_t& color) {
        _data = color;
        return *this;
    }
    GraphicRGBA& operator=(const GraphicRGBA& color) = default;
    GraphicRGBA& operator=(GraphicRGBA&& color) = default;

    static GraphicRGBA makeGraphicRGBA(const std::string& str) {
        constexpr uint32_t stringLen = 9;
        if ((str.size() != stringLen && str.size() != stringLen - 2) || str[0] != '#') {
            return {};
        }
        uint32_t data = 0;
        constexpr uint32_t offset = 4;
        for (uint32_t i = 1; i < str.size(); i++) {
            data <<= offset;
            data += str[i] < 'A' ? str[i] - '0' : str[i] - '7';
        }
        return GraphicRGBA(data);
    }

private:
    uint32_t _data = 0xFFFFFFFF;
};
} // namespace ULGui

#endif // _GRAPHIC2DTYPE_H
