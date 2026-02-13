#ifndef _CANVAS_MATRIX_H
#define _CANVAS_MATRIX_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>

namespace ULGui::canvas {
template<typename T>
class Matrix final {
public:
    Matrix(const T* matrix, uint32_t row, uint32_t col)
        : _row(row), _col(col) {
        assert(row > 0 && col > 0);
        uint32_t size = row * col;
        _matrix       = new T[size];
        memcpy(_matrix, matrix, sizeof(T) * size);
    }
    Matrix(const Matrix& obj) { copy(obj); }
    Matrix(Matrix&& obj) noexcept { move(std::move(obj)); }
    ~Matrix() { delete[] _matrix; }

    Matrix& operator=(const Matrix& obj) {
        delete[] _matrix;
        copy(obj);
        return *this;
    }
    Matrix& operator=(Matrix&& obj) noexcept {
        delete[] _matrix;
        move(std::move(obj));
        return *this;
    }

    const T* data() const { return _matrix; }
    T* data() { return _matrix; }
    bool empty() const { return _matrix == nullptr; }
    uint32_t row() const { return _row; }
    uint32_t col() const { return _col; }
    uint32_t size() const { return _row * _col; }
    const T get(uint32_t row, uint32_t col) const { return _matrix[index(row, col)]; }
    void set(uint32_t row, uint32_t col, const T& value) { _matrix[index(row, col)] = value; }
    void set(uint32_t row, uint32_t col, T&& value) { _matrix[index(row, col)] = value; }
    template<typename... Args>
    void set(uint32_t row, uint32_t col, Args&&... args) {
        _matrix[index(row, col)] = T(std::forward<Args>(args)...);
    }

protected:
    uint32_t index(uint32_t row, uint32_t col) const {
        assert(row < _row && col < _col);
        return _col * row + col;
    }
    void copy(const Matrix& obj) {
        uint32_t size = obj._row * obj._col;
        _matrix       = new T[size];
        memcpy(_matrix, obj._matrix, sizeof(T) * size);
        _row = obj._row;
        _col = obj._col;
    }
    void move(Matrix&& obj) noexcept {
        _matrix = obj._matrix;
        _row    = obj._row;
        _col    = obj._col;

        obj._matrix = nullptr;
        obj._row    = 0;
        obj._col    = 0;
    }

private:
    T* _matrix {nullptr};
    uint32_t _row {0};
    uint32_t _col {0};
};

using MatrixF = Matrix<float>;
using MatrixI = Matrix<int32_t>;
} // namespace ULGui::canvas

#endif // !_CANVAS_MATRIX_H
