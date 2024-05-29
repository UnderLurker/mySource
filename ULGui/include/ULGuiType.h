//
// Created by XW on 2024/5/16.
//

#ifndef _GRAPHIC2DTYPE_H
#define _GRAPHIC2DTYPE_H

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <cstring>

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

__ULGUI_DECLARE struct GraphicRGBA {
    float red {255.0f};
    float green {255.0f};
    float blue {255.0f};
    float alpha {255.0f};
    explicit GraphicRGBA(float r, float g, float b, float a = 255.0f)
        : red(r), green(g), blue(b), alpha(a) {}
};
} // namespace ULGui

#endif // _GRAPHIC2DTYPE_H
